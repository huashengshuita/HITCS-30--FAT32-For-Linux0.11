#ifndef LINGIX_STRING_H
#define LINGIX_STRING_H

extern inline void *memset (void *b, int c, unsigned long len)
  {
    unsigned char *p;
    register unsigned long i;

    p = (unsigned char *) b;
    for (i=0; i<len; i++)
	  p[i] = c;
    return b;
  }

extern inline void *memcpy(void *dst_ptr, const void *src_ptr, unsigned int count) 
{
	void *ret_val = dst_ptr;
	const char *src = (const char *)src_ptr;
	char *dst = (char *)dst_ptr;

 //copy up 
	for(; count != 0; count--)
		*dst++ = *src++;
	return ret_val;
}



extern inline int strlen(const char * s)
{
   int aLen=0;
   while(*s!=0x00)
   {
	   aLen++;
	   s++;
   }
   return aLen;
}




extern inline int strcmp(const char * s1, const char * s2)
{
	int i, ret;

	ret = 0;
	for (i = 0; s1[i] != '\0' || s2[i] != '\0'; i++) {
		if(s1[i] != s2[i]){
			ret = s1[i] > s2[i] ? 1 : -1;
			break;
		}
	}

	return ret;
}

extern inline int strncmp(const char * cs, const char * ct, int count)
{
	int i, ret;

	for (i = 0; i < count; i++) {
		if (cs[i] != ct[i]) {
			ret = cs[i] > ct[i] ? 1 : -1;
			return ret;
		}
	}
	return 0;
}


extern inline char * strcpy(char *dest, const char *src)
{
	while (*src)
	{
		*dest++ = *src++;
	}

	dest--;
  *dest='\0';
	return dest;
}


extern inline char *strcat(char *dest, const char *src)
{
	int i, j;

	for (i = 0; dest[i] != '\0'; i++)
		;

	for (j = 0; src[j] != '\0'; j++) {
		dest[i] = src[j];
		i++;
	}
	dest[i + 1] = '\0';
	return dest;
}


extern inline char *strncat(char *dest, const char *src, int maxlen)
{
	int i, j;

	for (i = 0; dest[i] != '\0'; i++)
		;

	for (j = 0; j < maxlen; j++) {
		dest[i] = src[j];
		i++;
	}
	dest[i + 1] = '\0';

	return dest;
}


extern inline const char *strchr(const char *s, int c)
{
	int i;

	for (i = 0; s[i] != '\0'; i++) {
		if (s[i] == c)
			return &s[i];
	}
	return '\0';
}

extern inline const char *strrchr(const char *s, int c)
{
	int i, len;

	len = strlen(s);

	for (i = len; s[i] != '\0'; i--) {
		if (s[i] == c)
			return &s[i];
	}
	return '\0';
}


extern inline const char *strstr(const char *s1, const char *s2)
{
	int i, j;

	i = 0;

	for (j = 0; s1[j] != '\0'; j++) {
		if (s1[j] == s2[i]) {
			if (strcmp(&s1[j], s2) == 0)
				return &s1[j];
		}
	}

	return '\0';
}

extern void sprintf(char* buf,const char * fmt, ...);


extern inline void add_char_to_str(char* str,unsigned char ch)
{
     while(*str) str++;
     *str++=ch;
     *str=0;
}

extern inline void del_char_from_str(char* str)
{
	 if(strlen(str))
     while(*str) str++;
	 str--;
     *str=0;
}

#endif /* LINGIX_STRING_H */
