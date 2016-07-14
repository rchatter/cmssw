#include "FWCore/Utilities/interface/Exception.h"
#include "Utilities/DavixAdaptor/interface/DavixFile.h"
#include "Utilities/StorageFactory/interface/StorageFactory.h"
#include "Utilities/StorageFactory/interface/StorageMaker.h"
#include "Utilities/StorageFactory/interface/StorageMakerFactory.h"
#include <davix.hpp>
#include <unistd.h>

class DavixStorageMaker : public StorageMaker {

public:
  /** Open a storage object for the given URL (protocol + path), using the
      @a mode bits.  No temporary files are downloaded.  */
  virtual std::unique_ptr<Storage> open(const std::string &proto, const std::string &path, int mode,
                                        AuxSettings const &aux) const override {
    const StorageFactory *f = StorageFactory::get();
    std::string newurl((proto == "web" ? "http" : proto) + ":" + path);
    auto file = std::make_unique<DavixFile>(newurl, mode);
    return f->wrapNonLocalFile(std::move(file), proto, std::string(), mode);
  }

  virtual bool check(const std::string &proto, const std::string &path, const AuxSettings &aux,
                     IOOffset *size = 0) const override {
    std::string newurl((proto == "web" ? "http" : proto) + ":" + path);
    Davix::DavixError *err = NULL;
    std::unique_ptr<Davix::DavPosix> davixPosix =
        std::make_unique<Davix::DavPosix>(new Davix::Context());
    Davix::StatInfo info;
    davixPosix->stat64(NULL, newurl, &info, &err);
    if (err) {
      std::unique_ptr<Davix::DavixError> davixErrManaged(err);
      std::cout << "Check failed with error " << err->getErrMsg().c_str() << " and error code"
                << err->getStatus() << std::endl;
      cms::Exception ex("FileCheckError");
      ex << "Check failed with error " << err->getErrMsg().c_str() << " and error code"
         << err->getStatus();
      ex.addContext("Calling DavixFile::check()");
      throw ex;
    }
    std::cout << "File is " << info.size << " bytes large" << std::endl;
    if (size) {
      *size = info.size;
    }
    return true;
  }
};

DEFINE_EDM_PLUGIN(StorageMakerFactory, DavixStorageMaker, "http");
DEFINE_EDM_PLUGIN(StorageMakerFactory, DavixStorageMaker, "https");
DEFINE_EDM_PLUGIN(StorageMakerFactory, DavixStorageMaker, "web");
