#ifndef _MESSAGE_DER_
#define _MESSAGE_DER_

#ifdef __cplusplus
extern "C" {
#endif

#define ITCASTDER_NoErr 0 

typedef int					INT ;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned long		UINT32;
typedef signed long         SINT32;

#define  TRUE  1
#define  FALSE 0


#define	   ITDER_NoErr                    0	
#define	   ITDER_MemoryErr                200
#define	   ITDER_LengthErr                201
#define	   ITDER_LengthNotEqualErr        202
#define	   ITDER_DataRangeErr             203
#define	   ITDER_InvalidTagErr            204



/* The ID's for universal tag numbers 0-31.  Tag number 0 is reserved for
   encoding the end-of-contents value when an indefinite-length encoding
   is used */
enum {
        
	    DER_ID_RESERVED,
		DER_ID_BOOLEAN, 
		DER_ID_INTEGER,
		DER_ID_BITSTRING,
		DER_ID_OCTETSTRING,
		DER_ID_NULL,
		DER_ID_OBJECT_IDENTIFIER,
		DER_ID_OBJECT_DESCRIPTOR,
		DER_ID_EXTERNAL,
		DER_ID_REAL,
		DER_ID_ENUMERATED, 
		DER_ID_EMBEDDED_PDV, 
		DER_ID_STRING_UTF8, 
		DER_ID_13,
		DER_ID_14, 
		DER_ID_15, 
		DER_ID_SEQUENCE, 
		DER_ID_SET,
		DER_ID_STRING_NUMERIC, 
		DER_ID_STRING_PRINTABLE, 
		DER_ID_STRING_T61,
		DER_ID_STRING_VIDEOTEX, 
		DER_ID_STRING_IA5, 
		DER_ID_TIME_UTC,
		DER_ID_TIME_GENERALIZED, 
		DER_ID_STRING_GRAPHIC, 
		DER_ID_STRING_ISO646,
		DER_ID_STRING_GENERAL, 
		DER_ID_STRING_UNIVERSAL, 
		DER_ID_29,
		DER_ID_STRING_BMP
		
		};

/* Whether the encoding is constructed or primitive */

#define DER_CONSTRUCTED			    0x20
#define DER_PRIMITIVE			    0x00

/* The encodings for the universal types */
#define DER_UNIVERSAL			    0x00
#define DER_APPLICATION		   	    0x40
#define DER_CONTEXT_SPECIFIC	        0x80
#define DER_PRIVATE				    0xC0

#define DER_RESERVED		      ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_RESERVED )
#define DER_BOOLEAN			  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_BOOLEAN )
#define DER_INTEGER			  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_INTEGER )
#define DER_BITSTRING		  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_BITSTRING )
#define DER_OCTETSTRING		  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_OCTETSTRING )
#define DER_NULL			      ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_NULL )
#define DER_OBJECT_IDENTIFIER  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_OBJECT_IDENTIFIER )
#define DER_OBJECT_DESCRIPTOR  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_OBJECT_DESCRIPTOR )
#define DER_EXTERNAL		      ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_EXTERNAL )
#define DER_REAL			      ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_REAL )
#define DER_ENUMERATED		  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_ENUMERATED )
#define DER_EMBEDDED_PDV	      ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_EMBEDDED_PDV )
#define DER_STRING_UTF8		  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_UTF8 )
#define DER_13				  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_13 )
#define DER_14				  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_14 )
#define DER_15				  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_15 )
#define DER_SEQUENCE		      ( DER_UNIVERSAL | DER_CONSTRUCTED | DER_ID_SEQUENCE )
#define DER_SET				  ( DER_UNIVERSAL | DER_CONSTRUCTED | DER_ID_SET )
#define DER_STRING_NUMERIC	  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_NUMERIC )
#define DER_STRING_PRINTABLE   ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_PRINTABLE )
#define DER_STRING_T61		  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_T61 )
#define DER_STRING_VIDEOTEX	  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_VIDEOTEX )
#define DER_STRING_IA5		  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_IA5 )
#define DER_TIME_UTC		      ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_TIME_UTC )
#define DER_TIME_GENERALIZED   ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_TIME_GENERALIZED )
#define DER_STRING_GRAPHIC	  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_GRAPHIC )
#define DER_STRING_ISO646	  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_ISO646 )
#define DER_STRING_GENERAL	  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_GENERAL )
#define DER_STRING_UNIVERSAL   ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_UNIVERSAL )
#define DER_29				  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_BER29 )
#define DER_STRING_BMP		  ( DER_UNIVERSAL | DER_PRIMITIVE | DER_ID_STRING_BMP )


/* Masks to extract information from a tag number */

#define DER_CLASS_MASK			0xC0
#define DER_CONSTRUCTED_MASK	    0x20
#define DER_SHORT_ID_MASK		0x1F
#define DER_FIRST_NOT_ID_MASK    0x7F     //xia
#define DER_FIRST_YES_ID_MASK    0x80     //xia
#define DER_ALL_YES_ID_MASK      0xFF     //xia
/* The maximum size for the short tag number encoding, and the magic value
   which indicates that a long encoding of the number is being used */

#define ITASN1_MAX_SHORT_BER_ID	    30
#define ITASN1_LONG_BER_ID		    0x1F

   
#define DER_Free(memblock) {if(memblock) {free(memblock); memblock=NULL;}}  

#define DER_ITASN1_LOW_IDENTIFIER(cTag)      (cTag & DER_SHORT_ID_MASK)

#define  DER_CREATE_LOW_ANYBUF(point) \
	     point = (ANYBUF *)malloc(sizeof(ANYBUF)); \
         if (point==NULL) return 7002; \
		 memset(point,0,sizeof(ANYBUF)); \
         point->dataLen = 0; \
         point->dataType = 0; \
	     point->memoryType = DER_ITASN1_MEMORY_MALLOC; \
	     point->pData = NULL; \
	     point->unusedBits = 0; \
	     point->next = NULL; \
         point->prev = NULL

#define DER_ITASN1_LOW_CREATEUINT8(point,size) \
	    point = malloc(size); \
		if (point==NULL) return 7002; \
        //memset(point,0,size)

#define DER_ITASN1_LOW_CREATEUINT32(point) \
	    point = malloc(sizeof(UINT32)); \
		if (point==NULL) return 7002; \
        memset(point,0,sizeof(UINT32))

#define DER_ITASN1_LOW_CHECKERR(iResult,iErrNumber) \
	    if (iResult == iErrNumber) \
            return iErrNumber
#define DER_DACERT_LOW_FREE_ANYBUF(point) \
	    DER_Free(point->pData); \
        DER_Free(point)
#define DER_ITASN1_MEMORY_STATIC     1
#define DER_ITASN1_MEMORY_MALLOC     2

typedef struct ANYBUF_{
	
    unsigned char 	  *pData;
    UINT32     dataLen;
    
    UINT32     unusedBits;  /* for bit string */  
    UINT32     memoryType;
    UINT32     dataType;
    struct ANYBUF_ *next;    /* for sequence and set */
    struct ANYBUF_ *prev;
    
}ANYBUF;
 
typedef UINT8     ITASN1_BOOLEAN;
typedef ANYBUF    ITASN1_INTEGER;
typedef ANYBUF    ITASN1_OCTETSTRING;
typedef ANYBUF    ITASN1_BITSTRING;
typedef ANYBUF    ITASN1_PRINTABLESTRING;
/*begin of bmpstring*/
typedef ANYBUF	 ITASN1_BMPSTRING;
/*end of bmpstring*/
typedef ANYBUF    ITASN1_ENUMERATED;
typedef ANYBUF    ITASN1_IA5STRING;
typedef ANYBUF    ITASN1_SEQUENCE;
typedef ANYBUF    ITASN1_SET; 
//typedef time_t       ITASN1_GENERALIZEDTIME;
//typedef time_t       ITASN1_UTCTIME;


INT
DER_ASN1_WriteInteger(UINT32 integer, ITASN1_INTEGER **ppDerInteger);

INT
DER_ASN1_ReadInteger(ITASN1_INTEGER *pDerInteger, UINT32 *pInteger);

INT
DER_ASN1_WriteBitString(ITASN1_BITSTRING *pBitString,ITASN1_BITSTRING **ppDerBitString);

INT
DER_ASN1_ReadBitString(ITASN1_BITSTRING *pDerBitString,ITASN1_BITSTRING **ppBitString);

INT
DER_ASN1_WritePrintableString(ITASN1_PRINTABLESTRING *pPrintString, ITASN1_PRINTABLESTRING **ppDerPrintString);

INT
DER_ASN1_ReadPrintableString(ITASN1_PRINTABLESTRING *pDerPrintString, ITASN1_PRINTABLESTRING **ppPrintString);

INT
DER_ASN1_WriteSequence(ITASN1_SEQUENCE *pSequence, ANYBUF **ppDerSequence);

INT
DER_ASN1_ReadSequence(ANYBUF *pDerSequence, ITASN1_SEQUENCE **ppSequence);

INT
DER_ASN1_WriteNull(ANYBUF ** ppDerNull);

INT
DER_ASN1_ReadNull(ANYBUF * ppDerNull, UINT8 * pInt);

INT 
DER_FreeQueue(ANYBUF *pAnyBuf);

INT
DER_String_To_AnyBuf(ANYBUF **pOriginBuf,	unsigned char * strOrigin,int strOriginLen);

int WriteNullSequence(ANYBUF **pOutData);

int EncodeChar(char *pData, int dataLen, ANYBUF **outBuf);

int EncodeUnsignedChar(unsigned char *pData, int dataLen, ANYBUF **outBuf);

int DecodeChar(ANYBUF *inBuf, char **Data, int *pDataLen);

#ifdef __cplusplus
}
#endif

#endif
