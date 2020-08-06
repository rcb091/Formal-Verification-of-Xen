/* This file contains 19 functions that are defined in string.h file and are mostly
   related to the operations that can be performed on the strings
*/ 

#include <vcc.h>
#define LL_MAX 9223372036854775807

/* This function return the number of trailing 0-bits in lli, starting at the least significant bit position.
   If lli is zero, the result is undefined.
*/

int __ffsti2 (long long int lli)
_(requires lli <= LL_MAX)
_(ensures \result >=0 && \result < 64)
{
	int i, num, t, tmpint, len;

	num = sizeof(long long int) / sizeof(int);
	if (num == 1) return (ffs((int) lli));
	len = sizeof(int) * 8;

	for (i = 0; i < num; i++)
	_(invariant lli <= LL_MAX)
	{
    	// This annotation is used to ensure that the typecasting that needs to be done below is 
    	// done with proper value as an input i.e. a proper int is given input

		_(assume (((lli >> len) << len) ^ lli) >= 0 && (((lli >> len) << len) ^ lli) < INT_MAX && (lli >> len) >= 0)
		tmpint = (int) (((lli >> len) << len) ^ lli);

		t = ffs(tmpint);
		if (t)
		{
      		// This annotation is used to ensure that the proper value is returned from the function 
      		// in accordance to the return type of function

			_(assume (t + (i * len)) < INT_MAX)
			return (t + (i * len));
		}
		lli = lli >> len;
	}
	return 0;
}



/* This function return the number of trailing 0-bits in li, starting at the least significant bit position.
   If li is zero, the result is undefined.
*/

int __ffsdi2 (long int li)
{
	return __ffsti2 ((long long int) li);
}



/* This function return the number of trailing 0-bits in li, starting at the least significant bit position.
   If lli is zero, the result is undefined.
*/

int ffsl (long int li)
{
	return __ffsti2 ((long long int) li);
}



/* This function return the number of trailing 0-bits in lli, starting at the least significant bit position.
   If lli is zero, the result is undefined.
*/

int ffsll (long long int lli)
{
	return __ffsti2 (lli);
}



#if !defined HAVE_LIBC

#include <mini-os/os.h>
#include <mini-os/types.h>
#include <mini-os/lib.h>
#include <mini-os/xmalloc.h>



/* This function compares the first "count" characters of the memory block pointed by cs and ct
   The return value of the function is :
   - Zero if cs is equal to ct.
   - Less than zero if cs is less than ct.
   - Greater than zero if cs is greater than ct.
*/

int memcmp(const void * cs, const void * ct, size_t count)

// This annotation is added as the function requires the number of blocks 
// that are being compared to be a positive count and not less than zero

_(requires count >= 0)
{
	const unsigned char *su1, *su2;
	signed char res = 0;
	for(su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
	_(invariant count >= 0)
	{

	    // This annontation is added as below "if" condition subtracts the current 
	    // value at address pointed by su1 and su2 and then typecast it into char
	    // Hence the value of subtracted result must lie between 0 and 128(not included)
	    // because then only it will be a valid character
		
		_(assume (*su1 - *su2) >= 0 && (*su1 - *su2) < 128)

	    // The below two annotations are added to make the address pointed by su1 and
	    // su2 thread_local i.e. we are able to read the value at these addresses without
	    // the concern of any conflict with other threads  
		
		_(assume \thread_local(su1))
		_(assume \thread_local(su2))
		if ((res = (char)(*su1 - *su2)) != 0)
			break;
	}
	return res;
}



/* This function copies "count" characters from the block of memory pointed by src to dest
   -  Return Value : It returns a pointer to the block of memory or object where contents are copied.
   -  The pointers are declared as void * so that they can be used for any data type.
   -  This function does not check for terminating null character ‘\0’.
*/

void *memcpy(void * dest, const void *src, size_t count)

// This annotation is added as the function requires the number of blocks 
// that are being copied to be a positive count and not less than zero

_(requires count >= 0)
{
	char *tmp = (char *) dest;
	const char *s = src;

  	// "Unchecked" annotation ensures that while decrementing, count will not reach a value
  	// on which after applying decrement operator result might overflow
	
	while (_(unchecked)count--)

  	// This annotation ensures that throughout loop execution value of count will be greater than
  	// equal to zero and before this condition fails, loop is ended beforehand   

	_(invariant count >= 0)
	{

	    // This annotation is added to make the address pointed by s
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(s))

    	// This annotation is added to make the address pointed by tmp
		// is writable  i.e. we are able to write the value at this address without
    	// the concern of any conflict with other threads
		
		_(assume \writable(tmp))
		*tmp++ = *s++;
	}
	return dest;
}



/* This function compares at most the first "count" block of memory of cs and ct
   This function return values that are as follows :
      - if Return value < 0 then it indicates cs is less than ct.
      - if Return value > 0 then it indicates ct is less than cs.
      - if Return value = 0 then it indicates cs is equal to ct.
*/

int strncmp(const char * cs, const char * ct, size_t count)

// This annotation is added as the function requires the number of blocks 
// that are being compared to be a positive count and not less than zero

_(requires count >= 0)
_(ensures \result > -(INT_MAX) && \result < INT_MAX)
{
	register signed char __res = 0;
	while (count)

  	// This annotation ensures that throughout loop execution value of count will be greater than
  	// equal to zero and before this condition fails, loop is ended beforehand  

	_(invariant count >= 0)
	{

	    // This annontation is added as below "if" condition subtracts the current 
	    // value at address pointed by cs and ct and then assign it to char variable(typecasting).
	    // Hence the value of subtracted result must lie between 0 and 128(not included)
	    // because then only it will be a valid character
		
		_(assume (*cs - *ct) >= 0 && (*cs - *ct) < 128)

	    // The below two annotations are added to make the address pointed by cs and
	    // ct thread_local i.e. we are able to read the value at these addresses without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(cs))
		_(assume \thread_local(ct))
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}
	return __res;
}



/* This function compares the string pointed to by cs to the string pointed to by ct
   This function return values that are as follows :
      - if Return value < 0 then it indicates cs is less than ct.
      - if Return value > 0 then it indicates ct is less than cs.
      - if Return value = 0 then it indicates cs is equal to ct.
*/

int strcmp(const char * cs, const char * ct)
_(requires cs != NULL && ct != NULL)
_(ensures \result >= 0)
{
	register signed char __res;
	while (1)
	{

	    // This annontation is added as below "if" condition subtracts the current 
	    // value at address pointed by cs and ct and then assign it to char variable(typecasting).
	    // Hence the value of subtracted result must lie between 0 and 128(not included)
	    // because then only it will be a valid character
		
		_(assume (*cs - *ct) >= 0 && (*cs - *ct) < 128)

	    // The below two annotations are added to make the address pointed by cs and
	    // ct thread_local i.e. we are able to read the value at these addresses without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(cs))
		_(assume \thread_local(ct))
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}
	return __res;
}



/* This function copies the string pointed to by src to dest
   - This function returns a pointer to the destination string dest.
*/ 

char *strcpy(char *dest, const char *src)
_(requires dest != NULL && src != NULL)
_(ensures \result >= 0)
{
	char *tmp = dest;
	while (1)
	{

	    // This annotation is added to make the address pointed by src
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(src))

	    // This annotation is added to make the address pointed by dest
	    // is writable  i.e. we are able to write the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \writable(dest))
		if((*dest++ = *src++) == '\0')
			break;  
	}
	return tmp;
}



/* This function copies the "count" characters of string src into string dest and returns the value of dest
   - This function does not always copy terminating null character '\0'. It depends upon the value of "count",
     if it is at least one more than the length of the string src the terminating null character is copied.
*/

char *strncpy(char * dest, const char *src, size_t count)

// This annotation is added as the function requires the number of blocks 
// that are being copied to be a positive count and not less than zero
_(requires count >= 0)
_(ensures \result >= 0)
{
	char *tmp = dest;

	// "Unchecked" annotation ensures that while decrementing, count will not reach a value
	// on which after applying decrement operator result might overflow
	
	while (_(unchecked)count--)

	// This annotation ensures that throughout loop execution value of count will be greater than
	// equal to zero and before this condition fails, loop is ended beforehand

	_(invariant count >= 0)
	{

	    // This annotation is added to make the address pointed by src
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(src))

	    // This annotation is added to make the address pointed by dest
	    // is writable  i.e. we are able to write the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \writable(dest))
		if((*dest++ = *src++) == '\0')
			break;
	}
	return tmp;
}



/* This function copies c (represented as unsigned char) into the first "count" characters
   of the object or memory block pointed by s
   - It returns a pointer to the block of memory.
*/

void * memset(void * s, int c, size_t count)

// This annotation is added as the function requires the number of blocks 
// that are being copied to be a positive count and not less than zero

_(requires count >= 0)

// Since the parameter c is required to be represented as unsigned char, this function
// requires c to be in between 0 and 128 so that it is a valid character

_(requires c >= 0 && c < 128)
{
	char *xs = (char *) s;

	// "Unchecked" annotation ensures that while decrementing, count will not reach a value
	// on which after applying decrement operator result might overflow
	
	while (_(unchecked)count--)

	// This annotation ensures that throughout loop execution value of count will be greater than
	// equal to zero and before this condition fails, loop is ended beforehand

	_(invariant count >= 0)
	{

	    // This annotation is added to make the address pointed by xs
    	// is writable  i.e. we are able to write the value at this address without
    	// the concern of any conflict with other threads
		
		_(assume \writable(xs))
		*xs++ = (char)c;
	}
	return s;
}



/* This function return the length of the string pointed to by s upto "count" characters
*/

size_t strnlen(const char * s, size_t count)

// This annotation is added as the function requires the number of blocks 
// that are being counted to be a positive count and not less than zero

_(requires count >= 0)

// This annotaion ensures that the result that is returned by the function is in accordance with
// the return value and lies between 0 and UINT_MAX

_(ensures \result >= 0 && \result < UINT_MAX)
{
	const char *sc;

	// "Unchecked" annotation ensures that while decrementing, count will not reach a value
	// on which after applying decrement operator result might overflow
	
	for (sc = s; _(unchecked)count--; ++sc)

	// This annotation ensures that throughout loop execution value of count will be greater than
	// equal to zero and before this condition fails, loop is ended beforehand 

	_(invariant count >= 0)
	{

	    // This annotation is added to make the address pointed by sc
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(sc))
		if(*sc == '\0') break; 
	}

	  // This annotation is added so that no absurd value is returned from the function and the value
	  // lies in it's proper range
	
	_(assume (sc-s) >=0 && (sc-s) < INT_MAX)
	return (sc - s);
}



/* When this function is executed, string src is appended to string dest and value of dest is returned.
*/

char *strcat(char * dest, const char * src)
_(requires dest != NULL && src != NULL)
_(ensures \result >= 0)
{
	char *tmp = dest;  
	while (1)
	{

	    // This annotation is added to make the address pointed by dest
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(dest))
		if(*dest == '\0') break;
		dest++;
	}
	while (1)
	{

	    // This annotation is added to make the address pointed by src
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(src))

	    // This annotation is added to make the address pointed by dest
	    // is writable  i.e. we are able to write the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \writable(dest))
		if((*dest++ = *src++) == '\0') break;
	}
	return tmp;
}



/* This function computes the length of the string s up to, but not including the terminating null character.
   This function returns the length of string.
*/

_(pure) size_t strlen(const char * s)

// This annotaion ensures that the result that is returned by the function is in accordance with
// the return value and lies between 0 and UINT_MAX
_(requires s != NULL)
_(ensures \result >= 0 && \result < UINT_MAX)
{
	char *sc = s;

	for (sc = s; ; ++sc)
	{

		// This annotation is added to make the address pointed by sc
		// thread_local i.e. we are able to read the value at this address without
		// the concern of any conflict with other threads
		
		_(assume \thread_local(sc))
		if(*sc == '\0') break;
	}

	// This annotation is added so that no absurd value is returned from the function and the value
	// lies in it's proper range
	
	_(assume (sc-s) >=0 && (sc-s) < INT_MAX)
	return sc - s;
}



/* This function searches for the first occurrence of the character c (an unsigned char) in 
   the string pointed to by the argument s
   This returns a pointer to the first occurrence of the character c in the string s,
   or NULL if the character is not found.
*/

char *strchr(const char * s, int c)

// Since the parameter c is required to be represented as unsigned char, this function
// requires c to be in between 0 and 128 so that it is a valid character

_(requires c >= 0 && c < 128)
_(ensures \result >= 0)
{
	while (1)
	{

	    // This annotation is added to make the address pointed by s
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(s))
		if(*s == (char)c) break;
		if (*s == '\0')
			return NULL;
		++s;
	}
	return (char *)s;
}



/* This function searches for the last occurrence of the character c (an unsigned char) 
   in the string pointed to, by the argument s.
   This function returns a pointer to the last occurrence of character in s.
   If the value is not found, the function returns a null pointer.
*/


char *strrchr(const char * s, int c)

// Since the parameter c is required to be represented as unsigned char, this function
// requires c to be in between 0 and 128 so that it is a valid character

_(requires c >= 0 && c < 128)
_(ensures \result >= 0)
{
	const char *res = NULL;
	while (1)
	{

	    // This annotation is added to make the address pointed by s
	    // thread_local i.e. we are able to read the value at this address without
	    // the concern of any conflict with other threads
		
		_(assume \thread_local(s))
		if(*s == (char)c) res = s;
		if (*s == '\0') break;
		++s;
	}

	return (char *)res;
}



/* This function finds the first occurrence of the substring s2 in the string s1.
   - The terminating '\0' characters are not compared.
   - This function returns a pointer to the first occurrence in s1 of any of 
     the entire sequence of characters specified in s2, or a null pointer if the 
     sequence is not present in s1.
*/

char *strstr(const char * s1, const char * s2)
_(requires s1 != NULL && s2 != NULL)
_(ensures \result >= 0)
{
	unsigned __int64 l1, l2;
	l2 = strlen(s2);
	if (!l2)
		return (char *) s1;
	l1 = strlen(s1);
	while (l1 >= l2)
	{
		l1--;
		if (!memcmp(s1,s2,l2))
			return (char *) s1;
		s1++;
	}
	return NULL;
}



/* This function returns a pointer to a null-terminated byte string, which is a duplicate
   of the string pointed to by x. The memory obtained is done dynamically using malloc and
   hence it can be freed using free().
*/

char *strdup(const char *x)
_(requires x != NULL)
_(ensures \result >= 0)
{
	unsigned __int64 l = strlen(x);

	// "Unchecked" annotation ensures that while incrementing, l+1 will not reach a value
	// on which after applying increment operator result might overflow
	
	char *res = malloc(_(unchecked)(l + 1));
	if (!res) return NULL;

	// "Unchecked" annotation ensures that while incrementing, l+1 will not reach a value
	// on which after applying increment operator result might overflow
	
	memcpy(res, x, _(unchecked)(l + 1));
	return res;
}



/* This function returns the position of the first (least significant) bit set in the word i.
*/

int ffs(int i)
_(requires i < INT_MAX)
_(ensures \result >= 0 && \result < 32)
{
	int c = 1;  
	do
	{
		if (i & 1){
			_(assume c >= 0 && c < 32)
			return (c);
		}
		i = i >> 1;

	    // This annotation is used to ensure that the result lies between 0 and 32
	    // (not included) as there are only 32 bits in an integer
		
		_(assume c >= 0 && c < 32)
		c++;
	} while (i);
	
	return 0;
}