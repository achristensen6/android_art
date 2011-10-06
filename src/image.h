// Copyright 2011 Google Inc. All Rights Reserved.

#ifndef ART_SRC_IMAGE_H_
#define ART_SRC_IMAGE_H_

#include <string.h>

#include "globals.h"
#include "object.h"

namespace art {

// header of image files written by ImageWriter, read and validated by Space.
class PACKED ImageHeader {
 public:
  ImageHeader() {}

  ImageHeader(uint32_t image_base_addr,
              uint32_t image_roots,
              uint32_t oat_checksum,
              uint32_t oat_base_addr,
              uint32_t oat_limit_addr)
      : image_base_addr_(image_base_addr),
        oat_checksum_(oat_checksum),
        oat_base_addr_(oat_base_addr),
        oat_limit_addr_(oat_limit_addr),
        image_roots_(image_roots) {
    CHECK_EQ(image_base_addr, RoundUp(image_base_addr, kPageSize));
    CHECK_EQ(oat_base_addr, RoundUp(oat_base_addr, kPageSize));
    CHECK_LT(image_base_addr, image_roots);
    CHECK_LT(image_roots, oat_base_addr);
    CHECK_LT(oat_base_addr, oat_limit_addr);
    memcpy(magic_, kImageMagic, sizeof(kImageMagic));
    memcpy(version_, kImageVersion, sizeof(kImageVersion));
  }

  bool IsValid() const {
    if (memcmp(magic_, kImageMagic, sizeof(kImageMagic) != 0)) {
      return false;
    }
    if (memcmp(version_, kImageVersion, sizeof(kImageVersion) != 0)) {
      return false;
    }
    return true;
  }

  const char* GetMagic() const {
    CHECK(IsValid());
    return reinterpret_cast<const char*>(magic_);
  }

  byte* GetImageBaseAddr() const {
    return reinterpret_cast<byte*>(image_base_addr_);
  }

  uint32_t GetOatChecksum() const {
    return oat_checksum_;
  }

  byte* GetOatBaseAddr() const {
    return reinterpret_cast<byte*>(oat_base_addr_);
  }

  byte* GetOatLimitAddr() const {
    return reinterpret_cast<byte*>(oat_limit_addr_);
  }

  enum ImageRoot {
    kJniStubArray,
    kAbstractMethodErrorStubArray,
    kInstanceResolutionStubArray,
    kStaticResolutionStubArray,
    kUnknownMethodResolutionStubArray,
    kCalleeSaveMethod,
    kOatLocation,
    kDexCaches,
    kClassRoots,
    kImageRootsMax,
  };

  Object* GetImageRoot(ImageRoot image_root) const {
    return reinterpret_cast<ObjectArray<Object>*>(image_roots_)->Get(image_root);
  }

 private:
  static const byte kImageMagic[4];
  static const byte kImageVersion[4];

  byte magic_[4];
  byte version_[4];

  // required base address for mapping the image.
  uint32_t image_base_addr_;

  // checksum of the oat file we link to for load time sanity check
  uint32_t oat_checksum_;

  // required oat address expected by image Method::GetCode() pointers.
  uint32_t oat_base_addr_;

  // end of oat address range for this image file, used for positioning a following image
  uint32_t oat_limit_addr_;

  // absolute address of an Object[] of objects needed to reinitialize from an image
  uint32_t image_roots_;

  friend class ImageWriter;
};

}  // namespace art

#endif  // ART_SRC_IMAGE_H_
