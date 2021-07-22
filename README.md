# `lib3gpp23038`

This is a set of functions to decode and encode the 7-bit GSM encoding, commonly known as [GSM 03.38](https://portal.3gpp.org/desktopmodules/Specifications/SpecificationDetails.aspx?specificationId=139), but these days defined in [3GPP TS 23.038](https://portal.3gpp.org/desktopmodules/Specifications/SpecificationDetails.aspx?specificationId=745).

As opposed to most other implementations, it supports the National Language Shift Tables as defined by the latest 3GPP specification at the time of writing.

The library is very small and meant to be integrated directly into your application. The supplied Makefile builds static and shared libraries only as an example.

# Legal

The licence of the library itself is available in `LICENCE.BSD`.

This software uses the Unicode Data File for mapping GSM 03.38 septets to Unicode characters, whose usage is governed under the following licence :

```
Copyright © 1991-2021 Unicode, Inc. All rights reserved.
Distributed under the Terms of Use in https://www.unicode.org/copyright.html.

Permission is hereby granted, free of charge, to any person obtaining
a copy of the Unicode data files and any associated documentation
(the "Data Files") or Unicode software and any associated documentation
(the "Software") to deal in the Data Files or Software
without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, and/or sell copies of
the Data Files or Software, and to permit persons to whom the Data Files
or Software are furnished to do so, provided that either
(a) this copyright and permission notice appear with all copies
of the Data Files or Software, or
(b) this copyright and permission notice appear in associated
Documentation.

THE DATA FILES AND SOFTWARE ARE PROVIDED "AS IS", WITHOUT WARRANTY OF
ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT OF THIRD PARTY RIGHTS.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS
NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL
DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THE DATA FILES OR SOFTWARE.

Except as contained in this notice, the name of a copyright holder
shall not be used in advertising or otherwise to promote the sale,
use or other dealings in these Data Files or Software without prior
written authorization of the copyright holder.
```
