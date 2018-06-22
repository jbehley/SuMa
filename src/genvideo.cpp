#include <rv/Laserscan.h>
#include <QtWidgets/QApplication>
#include <fstream>
#include <iostream>
#include "io/KITTIReader.h"
#include "util/kitti_utils.h"

#include "io/KITTIReader.h"
#include "visualizer/ViewportWidget.h"

#include <glow/glutil.h>
#include <rv/FileUtil.h>
#include <boost/filesystem.hpp>
#include <core/SurfelMapping.h>
#include "opengl/Model.h"
#include "opengl/ObjReader.h"
#include "util/ScanAccumulator.h"

using namespace rv;
using namespace glow;

std::shared_ptr<Model> initilizeKITTIrobot() {
  // FIXME: read model,extrinsic calibration, etc.  from settings.xml in scan
  // directory.

  if (!rv::FileUtil::exists("../assets/KIT_passat.obj")) {
    std::cerr << "Error: KIT robot model not found." << std::endl;
    return nullptr;
  }

  Eigen::Matrix4f T = glTranslate(0.5, 0, -1.0);

  std::shared_ptr<Model> model = std::make_shared<Model>();

  Mesh passat = ObjReader::fromFile("../assets/KIT_passat.obj");
  model->attach(T * glRotateZ(Radians(180.0f)) * glScale(5., 5., 5.), passat);

  Mesh tire = ObjReader::fromFile("../assets/KIT_tire.obj");
  model->attach(T * glScale(0.65, 0.65, 0.65) * glTranslate(2.27, -1.17, -0.86), tire);  // front right
  model->attach(T * glScale(0.65, 0.65, 0.65) * glTranslate(2.27, 1.17, -0.86) * glRotateZ(Radians(180.0f)),
                tire);  // front left
  model->attach(T * glScale(0.65, 0.65, 0.65) * glTranslate(-2.05, -1.17, -0.86), tire);
  model->attach(T * glScale(0.65, 0.65, 0.65) * glTranslate(-2.05, 1.17, -0.86) * glRotateZ(Radians(180.0f)), tire);

  return model;
}

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  setlocale(LC_NUMERIC, "C");

  if (argc < 3) {
    std::cerr << "Missing parameters: ./genvideo [parameter] [scan directory] [outputname] [<image output directory>]"
              << std::endl;
    return 1;
  }

  std::string outputFilename = argv[3];
  std::string outputDirectory;
  bool isTempDirectory = (argc < 5);
  outputDirectory = boost::filesystem::unique_path().string();
  if (argc == 5) outputDirectory = argv[4];

  if (!rv::FileUtil::exists(outputDirectory)) boost::filesystem::create_directories(outputDirectory);

  ParameterList params;
  parseXmlFile(argv[1], params);

  uint32_t historySize = 1000;
  uint32_t historyStride = 10;

  ViewportWidget widget;

  std::shared_ptr<ScanAccumulator> acc;
  acc = std::make_shared<ScanAccumulator>(1000, 150000);

  widget.initialize(params);
  widget.setFixedHeight(600);
  widget.setFixedWidth(600);

  widget.setDrawingOption("birds eye view", true);
  widget.setDrawingOption("remove ground", true);
  widget.setDrawingOption("colorize history", true);
  widget.setDrawingOption("pose graph", true);
  widget.setDrawingOption("draw history", true);
  widget.setDrawingOption("draw robot", true);
  widget.setDrawingOption("groundtruth", false);

  widget.setDrawingOption("normal map", false);

  widget.setRobotModel(initilizeKITTIrobot());

  widget.show();

  widget.setScanAccumualator(acc);
  widget.setScanCount(historySize);
  widget.setHistoryStride(historyStride);
  widget.setHistorySize(historySize);

  widget.updateGL();

  QApplication::processEvents();

  SurfelMapping fusion(params);

  uint32_t currentScanIdx = 0;

  Laserscan scan;
  KITTIReader reader(argv[2]);

  QStringList parts = QString::fromStdString(argv[2]).split("/");

  uint32_t N = reader.count();

  KITTICalibration calib;
  std::vector<Eigen::Matrix4f> groundtruth;

  calib.clear();
  groundtruth.clear();

  if (parts.size() > 2) {
    std::string calibration_file;
    for (int32_t i = 0; i < parts.size() - 2; ++i) {
      calibration_file += parts[i].toStdString();
      calibration_file += "/";
    }
    calibration_file += "calib.txt";

    std::cout << "calibration filename: " << calibration_file << "..." << std::flush;

    if (rv::FileUtil::exists(calibration_file)) {
      calib.initialize(calibration_file);
      std::cout << "loaded." << std::endl;

      widget.setCalibration(calib);
    } else {
      std::cout << "not found." << std::endl;
    }
  }

  if (parts.size() > 3 && calib.exists("Tr")) {
    std::string sequence = parts[parts.size() - 3].toStdString();

    Eigen::Matrix4f Tr = calib["Tr"];
    Eigen::Matrix4f Tr_inv = Tr.inverse();

    // find ground truth poses.
    std::string poses_file;
    for (int32_t i = 0; i < parts.size() - 4; ++i) {
      poses_file += parts[i].toStdString();
      poses_file += "/";
    }
    poses_file += "poses/" + sequence + ".txt";

    std::cout << "ground truth filename: " << poses_file << std::endl;
    if (rv::FileUtil::exists(poses_file)) {
      std::vector<Eigen::Matrix4f> poses = KITTI::Odometry::loadPoses(poses_file);

      for (uint32_t i = 0; i < poses.size(); ++i) {
        Eigen::Matrix4f pose = poses[i];

        groundtruth.push_back(Tr_inv * pose * Tr);
      }

      std::cout << groundtruth.size() << " poses read." << std::endl;
    } else {
      std::cout << "not found." << std::endl;
    }

    widget.setGroundtruth(groundtruth);
  }

  while (reader.read(scan)) {
    fusion.processScan(scan);

    //    currentScanIdx = fusion.timestamp();

    if (currentScanIdx % historyStride == 0) {
      std::vector<rv::Point3f> pts = scan.points();
      if (scan.hasRemission()) {
        for (uint32_t i = 0; i < pts.size(); ++i) {
          pts[i].vec[3] = scan.remission(i);
        }
      }
      acc->insert(currentScanIdx, fusion.getCurrentPose().cast<float>(), pts);
    }

    QApplication::processEvents();
    widget.setCurrentFrame(currentScanIdx, *fusion.getCurrentFrame());
    QApplication::processEvents();

    std::vector<Eigen::Matrix4d> optimizedPoses = fusion.getOptimizedPoses();

    // update poses in accumulator.
    std::vector<int32_t>& timestamps = acc->getTimestamps();
    std::vector<Eigen::Matrix4f>& acc_poses = acc->getPoses();

    for (uint32_t i = 0; i < timestamps.size(); ++i) {
      if (timestamps[i] > -1) {
        acc_poses[i] = optimizedPoses[timestamps[i]].cast<float>();
      }
    }

    widget.setOptimizedPoses(optimizedPoses);
    QApplication::processEvents();
    widget.setLaserscan(scan);

    QApplication::processEvents();
    widget.updateGL();
    QApplication::processEvents();

    std::string out_filename = outputDirectory;
    out_filename += QString("/%1.png").arg((int)currentScanIdx, 5, 10, (QChar)'0').toStdString();

    widget.grabFrameBuffer().save(QString::fromStdString(out_filename));
    std::cout << "Writing to " << out_filename << std::endl;

    QApplication::processEvents();
    currentScanIdx += 1;
  }

  std::cout << "Encoding video: " << std::endl;
  std::stringstream sstr;
  sstr << "ffmpeg -f image2 -i " << outputDirectory << "/%05d.png " << outputFilename << std::endl;

  if (std::system(sstr.str().c_str()) != 0) {
    std::cout << "Error: encoding failed!" << std::endl;
  }

  std::cout << "Finished." << std::endl;

  if (isTempDirectory) {
    std::cout << "Deleting temporary files." << std::endl;
    boost::filesystem::remove_all(outputDirectory);
  }

  return 0;
}
