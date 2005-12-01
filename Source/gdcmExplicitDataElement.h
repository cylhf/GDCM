
#ifndef __gdcmExplicitDataElement_h
#define __gdcmExplicitDataElement_h

/**
 * \brief Class to represent a Data Element
 * Implicit / Explicit
 * \note bla
 */

#include "gdcmDataElement.h"
#include "gdcmDICOMIStream.h"
#include "gdcmDICOMOStream.h"
#include "gdcmVR.h"

namespace gdcm
{
// Data Element (Explicit)
class ExplicitDataElement : public DataElement
{
public:
  ExplicitDataElement() { ValueLengthField = 0; VRField = VR::INVALID; SequenceLength = 0; }

  friend std::ostream& operator<<(std::ostream& _os, const ExplicitDataElement &_val);
  friend DICOMIStream& operator>>(DICOMIStream& _os, ExplicitDataElement &_val);
  friend DICOMOStream& operator<<(DICOMOStream& _os, const ExplicitDataElement &_val);

  uint32_t GetValueLength() { return ValueLengthField; }
  void SetValueLength(uint32_t vl) { ValueLengthField = vl; }

  uint32_t GetLength() { 
    assert( ValueLengthField != 0xFFFFFFFF 
      && ValueLengthField != 0xFFFF );
    return ValueLengthField; }

  VR::VRType GetVR() { return VRField; }
  void SetVR(VR::VRType vr) { VRField = vr; }

  uint32_t GetLength() const
    {
    //assert( ValueLengthField != 0xFFFFFFFF ); //FIXME
    // Nice trick each time VR::GetLength() is 2 then Value Length is coded in 2
    //                                         4 then Value Length is coded in 4
    if( ValueLengthField != 0xFFFFFFFF )
      return DataElement::GetLength() + 2*VR::GetLength(VRField) + ValueLengthField;
    else
      return DataElement::GetLength() + 2*VR::GetLength(VRField) + SequenceLength;
    }

private:
  // Value Representation
  VR::VRType VRField;
  // This is the value read from the file, might be different from the lenght of Value Field
  uint32_t ValueLengthField; // Can be 0xFFFFFFF

  // This is set if ValueLengthField == 0xFFFFFFF
  uint32_t SequenceLength;
};
//-----------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& _os, const ExplicitDataElement &_val)
{
  _os << _val.TagField << " VR=" << _val.VRField;
  if(_val.ValueLengthField == 0xFFFFFFF )
    _os << ",VL=" << std::dec << _val.ValueLengthField;
  else
    _os << ",VL=" << std::dec << _val.SequenceLength;
  const DataElement &de = _val;
  _os << " " << de;
  return _os;
}

} // end namespace gdcm

#endif //__gdcmExplicitDataElement_h
