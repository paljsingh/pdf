#!/bin/bash
if [ $# -eq 0 ]; then
    echo "usage: $0 image-file"
    exit 1
fi

count=1
for i in $* ; do
    stream=$(xxd -ps $i | tr -d '\n')
    length=$(echo -n "$stream" | wc -c | sed 's/ //g')
    width=$(exiftool $i  | grep '^Image Width' | awk -F": "  '{ print $2 }')
    height=$(exiftool $i  | grep '^Image Height' | awk -F": "  '{ print $2 }')
    bps=$(exiftool $i  | grep 'Bits Per Sample' | awk -F": "  '{ print $2 }')

cat <<EOFEOFEOF
$count 0 obj
<<
    /Length $length
    /Type /XObject
    /Subtype /Image
    /Width $width
    /Height $height
    /ColorSpace /DeviceRGB
    /BitsPerComponent $bps
    /Filter [ /ASCIIHexDecode /DCTDecode ]
>>
stream
$stream
endstream
endobj
EOFEOFEOF
    count=$(expr $count + 1)
    echo ; echo
done
