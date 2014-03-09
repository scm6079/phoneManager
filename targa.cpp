/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*   targa.cpp, simple .TGA file loader                                     */
/*   (c) Copyright TFPSoft, LLC. All rights reserved.                       */
/*                                                                          */
/*   Truevision Targa format created by Truevision Inc. All other           */  
/*   trademarks and copyrights are the property of their                    */
/*   respective owners.                                                     */
/*                                                                          */
/*   LANGUAGE: C/C++                                                        */
/*   PLATFORM: <generic>                                                    */
/*   EXTENDED LIBS\SDKS: <none>                                             */
/*                                                                          */
/*   LICENSE:                                                               */
/*   Permission to use, copy, modify, distribute, and sell this software    */
/*   and its documentation for any purpose is hereby granted without fee,   */
/*   provided that                                                          */
/*                                                                          */     
/*   (i) the above copyright notices and this permission notice appear      */
/*   in all copies of the program source code and related documentation,    */
/*   and                                                                    */
/*                                                                          */     
/*   (ii) the name of TFPSoft, LLC. may not be used in any advertising or   */
/*   publicity relating to the software without the specific, prior         */
/*   written permission of TFPSoft, LLC.                                    */
/*                                                                          */
/*   (iii) modified distributions must be clearly indicated, and must       */
/*   not be misrepresented as being the original software. Authors who      */
/*   modify the software are fully responsible for those modifications.     */
/*                                                                          */
/*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    */
/*   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      */
/*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  */
/*   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
/*   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  */
/*   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       */
/*   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  */
/*   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND         */
/*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,     */
/*   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT     */
/*   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY        */
/*   OF SUCH DAMAGE.                                                        */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
#include "targa.h"

/*---------------------------------------------------------------------------
   Loads a Targa file from disk

   Parameters: char *strFileName - file name of targa to load
                           WORD &wWidthOfImage - width of image
                           WORD &wHeightOfImage - height of image
                           WORD &wDepthInBytes - color depth in bytes
                           BYTE **pData - memory buffer

      Returns: true if success

            Notes: Only uncompressed 24 and 32-bit images are supported.
                           Caller is responsible for freeing memory buffer
---------------------------------------------------------------------------*/
bool LoadTarga(const char *strFileName, WORD &wWidthOfImage, WORD &wHeightOfImage, WORD &wDepthInBytes, BYTE **pData)
{
    FILE *file;                     // tga file handle 
    WORD wColorMapOrigin;           // origin of color map
    WORD wColorMapLength;           // length of color map
    BYTE wColorMapEntrySize;        // size of color map entry
    WORD wXOriginOfImage;           // origin of image on x axis
    WORD wYOriginOfImage;           // origin of image on y axis
    BYTE chColorMapType;            // type of color map
    BYTE chImageTypeCode;           // type of image (1 = palettized, 2 = RGB)
    BYTE chLengthOfImageIdField;    // length of image id field
    BYTE chImagePixelSizeInBits;    // bits per pixel
    BYTE chImageDescriptor;         // image descriptor
    BYTE *pCurrentPixel;            // current pixel in image data
    BYTE *pEndPixel;                // last pixel in image data
    DWORD dwBufferSize;             // size of the buffer in bytes
    BYTE yTemp;                     // temporary swap value
    BYTE *pSwap;                    // row swap buffer
    DWORD dwRowSize;                // size of row in bytes
    BYTE *pRow;                     // swap row

    // open file
    file = fopen(strFileName, "rb");
    if ( file == NULL ) {
        return false;
    }

    // read targa header
    chLengthOfImageIdField = 0;
    chColorMapType = 0;
    chImageTypeCode = 2;
    wColorMapOrigin = 0;
    wColorMapLength = 0;
    wColorMapEntrySize = 0;
    wXOriginOfImage = 0;
    wYOriginOfImage = 0;
    wWidthOfImage = 0;
    wHeightOfImage = 0;
    chImagePixelSizeInBits = 4 * 8;
    chImageDescriptor = 0;

    fread(&chLengthOfImageIdField, sizeof(chLengthOfImageIdField), 1, file);
    fread(&chColorMapType, sizeof(chColorMapType), 1, file);
    fread(&chImageTypeCode, sizeof(chImageTypeCode), 1, file);
    fread(&wColorMapOrigin, sizeof(wColorMapOrigin), 1, file);
    fread(&wColorMapLength, sizeof(wColorMapLength), 1, file);
    fread(&wColorMapEntrySize, sizeof(wColorMapEntrySize), 1, file);
    fread(&wXOriginOfImage, sizeof(wXOriginOfImage), 1, file);
    fread(&wYOriginOfImage, sizeof(wYOriginOfImage), 1, file);
    fread(&wWidthOfImage, sizeof(wWidthOfImage), 1, file);
    fread(&wHeightOfImage, sizeof(wHeightOfImage), 1, file);
    fread(&chImagePixelSizeInBits, sizeof(chImagePixelSizeInBits), 1, file);
    fread(&chImageDescriptor, sizeof(chImageDescriptor), 1, file);
    wDepthInBytes = chImagePixelSizeInBits / 8;

    // do not attempt to read anything other than 24 or 32 bit uncompressed data
    if ( (chImagePixelSizeInBits != 24) && (chImagePixelSizeInBits != 32) ) {
        fclose(file);
        return false;
    }
    if ( chImageTypeCode != 2 ) {
        fclose(file);
        return false;
    }

    // allocate storage
    dwBufferSize = (DWORD)wWidthOfImage * (DWORD)wHeightOfImage * (DWORD)wDepthInBytes;
    *pData = (BYTE *)malloc(dwBufferSize);
    if ( *pData == NULL ) {
        fclose(file);
        return false;
    }

    // read image
    fread(*pData, 1, dwBufferSize, file);

    // close file
    fclose(file);

    // swap rgb to bgr
    pCurrentPixel = *pData;
    pEndPixel = pCurrentPixel + dwBufferSize;
    do {
        yTemp = pCurrentPixel[0];
        pCurrentPixel[0] = pCurrentPixel[2];
        pCurrentPixel[2] = yTemp;
        pCurrentPixel += wDepthInBytes;
    } while ( pCurrentPixel != pEndPixel );

    // swap bottom up
    if ( (chImageDescriptor & 32) == 0 ) 
    {
       dwRowSize = wWidthOfImage * wDepthInBytes;
       pSwap = (BYTE *)malloc(dwRowSize);
       if ( pSwap == NULL ) {
           free(*pData);
           return false;
       }

       pCurrentPixel = *pData;
       pRow = pEndPixel - dwRowSize; 
       do {
           memcpy(pSwap, pCurrentPixel, dwRowSize);
           memcpy(pCurrentPixel, pRow, dwRowSize);
           memcpy(pRow, pSwap, dwRowSize);
           pRow -= dwRowSize;
           pCurrentPixel += dwRowSize;
       } while ( pCurrentPixel < pRow );

       free(pSwap);
    }

    // success
    return true;
}

