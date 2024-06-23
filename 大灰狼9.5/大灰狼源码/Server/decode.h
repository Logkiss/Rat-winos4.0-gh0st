static char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int pos(char c)
{
	char *p;
	for(p = base64; *p; p++)
		if(*p == c)
			return p - base64;
		return -1;
}

int base64_decode(const char *str, char **data)
{
	const char *s, *p;
	unsigned char *q;
	int c;
	int x;
	int done = 0;
	int len;
	s = (const char *)malloc(strlen(str));
	q = (unsigned char *)s;
	for(p=str; *p && !done; p+=4){
		x = pos(p[0]);
		if(x >= 0)
			c = x;
		else{
			done = 3;
			break;
		}
		c*=64;
		
		x = pos(p[1]);
		if(x >= 0)
			c += x;
		else
			return -1;
		c*=64;
		
		if(p[2] == '=')
			done++;
		else{
			x = pos(p[2]);
			if(x >= 0)
				c += x;
			else
				return -1;
		}
		c*=64;
		
		if(p[3] == '=')
			done++;
		else{
			if(done)
				return -1;
			x = pos(p[3]);
			if(x >= 0)
				c += x;
			else
				return -1;
		}
		if(done < 3)
			*q++=(c&0x00ff0000)>>16;
		
		if(done < 2)
			*q++=(c&0x0000ff00)>>8;
		if(done < 1)
			*q++=(c&0x000000ff)>>0;
	}
	
	len = q - (unsigned char*)(s);
	
	*data = (char*)realloc((void *)s, len);
	
	return len;
}

char* MyDecode(char *str)
{
	int		i, len;
	char	*data = NULL;
	len = base64_decode(str, &data);
	
	for (i = 0; i < len; i++)
	{
		// 		data[i] -= 0x83;
		// 		data[i] ^= 0x45;
		data[i] -= 0x73;
		data[i] ^= 0x23;
	}
	return data;
}