#ifndef __gdcmSequenceDataElement_txx
#define __gdcmSequenceDataElement_txx

#include "gdcmSequenceDataElement.h"

namespace gdcm
{
template<class DEType>
void SequenceDataElement<DEType>::AddSequenceItem(SequenceItem<DEType> const &item)
{
  SequenceItems.push_back(item);
}

//-----------------------------------------------------------------------------
template<class DEType>
DICOMOStream& operator<<(DICOMOStream& _os, const SequenceDataElement<DEType> &_val)
{
  (void)_val;
  assert( 0 );
  return _os;
}
//-----------------------------------------------------------------------------
template<class DEType>
DICOMIStream& operator>>(DICOMIStream& _os, SequenceDataElement<DEType> &_val)
{
  const Tag itemStart(0xfffe,0xe000); // Item
  const Tag itemEnd(0xfffe,0xe00d);
  const Tag seqDel(0xfffe,0xe0dd); //[Sequence Delimitation Item]
  SequenceItem<DEType> si; // = _val.SequenceItemField;
  assert( si.GetTag() == Tag(0,0) );
  DataElement &de = si;
  bool isBroken = false;
  if( _val.SequenceLengthField == 0xFFFFFFFF)
    {
    while( _os >> de )
      {
      if(de.GetTag() == seqDel) 
        {
        uint32_t length;
        _os.Read(length);
        // 7.5.2 Delimitation of the Sequence of Items
        // ...
        // b) Undefined Length: The Data Element Length Field shall contain a Value FFFFFFFFH to
        // indicate an Undefined Sequence length. It shall be used in conjunction with a Sequence
        // Delimitation Item. A Sequence Delimitation Item shall be included after the last Item
        // in the sequence. Its Item Tag shall be (FFFE,E0DD) with an Item Length of 00000000H. 
        // No Value shall be present.
        if( length != 0 )
          {
          std::cerr << "Wrong length for Sequence Delimitation Item: " << length; 
          abort();
          }
        // Looks like some pixel data have instead: == 0xFFFFFFFF -> SIEMENS-MR-RGB-16Bits.dcm
        break;
        }
      // else
      if( de.GetTag() != itemStart )
        {
        // gdcm-JPEG-LossLess3a.dcm
        std::streampos pos = _os.Tellg();
        _os.Seekg( 0, std::ios::end );
        std::streampos end = _os.Tellg();
        std::cerr << "Broken file: " << (long)(end-pos) 
          << " bytes were skipped at the end of file" << std::endl;
        isBroken = true;
        break;
        }
      _os >> si;
      _val.SequenceItems.push_back( si );
      if( si.GetLength() == 0xFFFFFFFF )
        {
        assert( de.GetTag() == itemEnd );
        }
      //_val.Length += si.GetLength();
      }
    if( !isBroken )
      {
      assert( si.GetTag() == seqDel );
      }
    }
  else
    {
    // Defined length, just read the SQItem
    std::cerr << "Debug: " << _val.SequenceLengthField << std::endl;
    uint32_t seq_length = 0;
    while( seq_length != _val.SequenceLengthField )
      {
      _os >> de;
      if( de.GetTag() != itemStart )
        {
        std::cerr << "BUGGY header" << std::endl;
        _os.Seekg( _val.SequenceLengthField - 4, std::ios::cur );
        break;
        }
      _os >> si;
      _val.SequenceItems.push_back( si );
      // Sequence Length = Item Tag Length + Sequence Value Length
      seq_length += de.GetTag().GetLength() + 4;
      seq_length += si.GetLength();
      std::cerr << "Debug: seq_length="  << seq_length << std::endl;
      assert( seq_length <= _val.SequenceLengthField );
      }
    }
  return _os;
}
}
#endif //__gdcmSequenceDataElement_txx

