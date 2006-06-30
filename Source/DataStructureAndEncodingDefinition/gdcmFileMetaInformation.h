
#ifndef __gdcmFileMetaInformation_h
#define __gdcmFileMetaInformation_h

#include "gdcmDataSet.h"
#include "gdcmTS.h"

namespace gdcm
{
/**
 * \brief Class to represent a File Meta Information
 * \note
 * In theory this should only be explicit, but just in case we also handle
 * the implicit case (illegal, but so usual)
 */
class ExplicitDataElement;
class ImplicitDataElement;
class GDCM_EXPORT FileMetaInformation
{
public:
  FileMetaInformation():DS(0) {}
  ~FileMetaInformation();

  friend std::ostream &operator<<(std::ostream &_os, const FileMetaInformation &_val);

  TS GetTransferSyntaxType() const;
  TS::MSType GetMediaStorageType() const;

  // Read
  IStream &Read(IStream &is);

  // Write
  OStream &Write(OStream &os) const;

private:
  bool ReadExplicitDataElement(IStream &is, ExplicitDataElement &de);
  bool ReadImplicitDataElement(IStream &is, ImplicitDataElement &de);

  DataSet *DS;
};
//-----------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream &os, const FileMetaInformation &val)
{
  if(val.DS)
    {
    const DataSet &ds = *(val.DS);
    os << ds;
    }
  else
    {
    os << "No File Meta Information Header";
    }
  return os;
}

} // end namespace gdcm

#endif //__gdcmFileMetaInformation_h

