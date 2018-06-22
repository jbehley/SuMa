
#ifndef KITTILASERSCANREADER_H_
#define KITTILASERSCANREADER_H_

#include "LaserscanReader.h"

#include <rv/ParameterList.h>
#include <rv/RingBuffer.h>
#include <rv/IOError.h>

namespace rv {

/** \brief a reader for the KITTI datasets provided by the KIT.
 *
 * \author behley
 */
class KITTIReader : public LaserscanReader {
 public:
  KITTIReader(const std::string& scan_filename, uint32_t buffer_size = 50);

  void reset() override;
  bool read(Laserscan& scan) override;
  void seek(uint32_t scan) override;
  bool isSeekable() const override;
  uint32_t count() const override;

 protected:
  void initScanFilenames(const std::string& scan_filename);

  bool read(uint32_t scan_idx, Laserscan& scan);

  int32_t currentScan;
  std::vector<std::string> scan_filenames;
  RingBuffer<Laserscan> bufferedScans;
  uint32_t firstBufferedScan;

};
}
#endif /* KITTILASERSCANREADER_H_ */
