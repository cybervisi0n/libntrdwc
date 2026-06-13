#include <nitro.h>
#include <nitroWiFi.h>
#include <string.h>
#include "auth/dwc_auth.h"
#include "auth/dwc_http.h"
#include "auth/util_alloc.h"
#include "auth/util_base64.h"
#include "decomp/decomp_defs.h"


char *DWCi_util_base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-";


int DWC_Auth_Base64Encode(const char *src, const unsigned long srclen, char *dst, const unsigned long dstlen)
{
	int required_dstsize, num_dstout, num_srcin;
	char *srcread_index, *tail_src, *dstwrite_index;
	unsigned char read_buffer[3];
	
	required_dstsize = (int)((srclen/3*4)+(srclen%3 != 0 ? 4 : 0));

	if(dst == NULL)
		return required_dstsize;
	
	if(dstlen < required_dstsize)
		return -1;
	
	srcread_index =	 (char *)src;
	tail_src = (char *)src+srclen;
	dstwrite_index = (char *)dst;
	
	while(srcread_index != tail_src) {
		num_dstout = (tail_src-srcread_index);
		num_dstout = num_dstout*8/6+(num_dstout*8%6 != 0 ? 1 : 0);
		
		num_srcin = tail_src-srcread_index < 3 ? tail_src-srcread_index : 3;
		MI_CpuFill8(read_buffer, 0x00, 3);
		MI_CpuCopy8(srcread_index, read_buffer, (unsigned long)num_srcin);
		
		*dstwrite_index = DWCi_util_base64_table[read_buffer[0] >> 2];
		
		dstwrite_index++;
		if(num_dstout >= 2)
			*dstwrite_index = DWCi_util_base64_table[(read_buffer[0] << 4) & 0x3f | (read_buffer[1] >> 4)];
		else
			*dstwrite_index = '*';
		
		dstwrite_index++;
		if(num_dstout >= 3)
			*dstwrite_index = DWCi_util_base64_table[(read_buffer[1] << 2) & 0x3f | (read_buffer[2] >> 6)];
		else
			*dstwrite_index = '*';
		
		dstwrite_index++;
		if(num_dstout >= 4)
			*dstwrite_index = DWCi_util_base64_table[read_buffer[2] & 0x3f];
		else
			*dstwrite_index = '*';
		
		dstwrite_index++;
		srcread_index += num_srcin;
	}
	return dstwrite_index-dst;
}

int DWC_Auth_Base64Decode(const char *src, const unsigned long srclen, char *dst, const unsigned long dstlen)
{
	char *srcread_index, *dstwrite_index;
	char read_buffer[4];
	
	int required_dstsize, i;
	
	if(srclen %4 != 0)
		return -1;
	
	required_dstsize = 0;
	for(i=0; i<srclen; i++) {
		if(src[i] != '*')
			required_dstsize += 6;
	}
	required_dstsize = required_dstsize/8;
	
	if(dst == NULL)
		return required_dstsize;
	
	if(dstlen < required_dstsize)
		return -1;
	
	if(srclen == 0) {
		dstwrite_index = (char *)dst;
		*dstwrite_index = 0x00;
		return 0;
	}
	
	srcread_index = (char *)src;
	dstwrite_index = (char *)dst;
	
	while(1) {
		for(i=0; i<4; i++) {
			if(srcread_index[i] >= 'A' && srcread_index[i] <= 'Z')
				read_buffer[i] = (char)(srcread_index[i]-'A');
			else if(srcread_index[i] >= 'a' && srcread_index[i] <= 'z')
				read_buffer[i] = (char)(srcread_index[i]-'a'+26);
			else if(srcread_index[i] >= '0' && srcread_index[i] <= '9')
				read_buffer[i] = (char)(srcread_index[i]-'0'+52);
			else if(srcread_index[i] == '.')
				read_buffer[i] = 62;
			else if(srcread_index[i] == '-')
				read_buffer[i] = 63;
			else
				read_buffer[i] = 0;
		}
		srcread_index += 4;
		
		*dstwrite_index = (char)((read_buffer[0] << 2) | (read_buffer[1] >> 4));
		dstwrite_index++;
		if(dstwrite_index-dst >= required_dstsize)
			break;
		
		*dstwrite_index = (char)((read_buffer[1] << 4) | (read_buffer[2] >> 2));
		dstwrite_index++;
		if(dstwrite_index-dst >= required_dstsize)
			break;
		
		*dstwrite_index = (char)((read_buffer[2] << 6) | read_buffer[3]);
		dstwrite_index++;
		if(dstwrite_index-dst >= required_dstsize)
			break;
	}
	return dstwrite_index-dst;
}