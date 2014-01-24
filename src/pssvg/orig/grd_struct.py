#!/usr/bin/env python
# (c) 2010, Valek Filippov

import sys,struct
shift_buf = "                                    "

def p_patt(buf,offset,name,shift):
# didn't rev.engineered yet
	return offset

def p_tdta(buf,offset,name,shift):
	[size] = struct.unpack('>L',buf[offset:offset+4])
	offset+=4
	string = buf[offset:offset+size]
	offset+=size
	print shift*" ",name,"(tdta",size,")",string
	return offset

def p_desc(buf,offset,name,shift):
	# convert 4 bytes as big-endian unsigned long
	[size] = struct.unpack('>L',buf[offset:offset+4])
	return offset+26

def p_long(buf,offset,name,shift):
	[size] = struct.unpack('>L',buf[offset:offset+4])
	print shift*" ",name,"(long)",size
	return offset+4

def p_vlls(buf,offset,name,shift):
	[size] = struct.unpack('>L',buf[offset:offset+4])
	offset+=4
	print shift*" ",name,"(VlLs)",size
	shift+=2
	for i in range(size):
		type = buf[offset:offset+4]
		offset+=4
		if types.has_key(type):
			offset = types[type](buf,offset,"----",shift)
		else:
			print "Unknown key:\t",name,type
			p_unkn(buf,offset,"",shift)
	shift-=2
	return offset

def p_objc(buf,offset,name,shift):
	[objnamelen] = struct.unpack('>L',buf[offset:offset+4])
	offset+=4
	objname = buf[offset:offset+objnamelen*2]
	offset+= objnamelen*2
	[objtypelen] = struct.unpack('>L',buf[offset:offset+4])
	if objtypelen == 0:
		objtypelen = 4
	offset+=4
	typename = buf[offset:offset+objtypelen]
	offset+=objtypelen
	[value] = struct.unpack('>L',buf[offset:offset+4])
	offset+=4
	print shift*" ",name,"(Objc)",objname,typename,value
	shift+=2
	for i in range(value):
		offset = parse_entry(buf,offset,shift)
	shift-=2
	return offset


def p_text(buf,offset,name,shift):
	[size] = struct.unpack('>L',buf[offset:offset+4])
	string = ""
	for i in range(size-1):
		string = string + str(buf[offset+4+i*2+1:offset+4+i*2+2])
	print shift*" ",name,"(TEXT",size,")",string
	return offset+4+size*2

def p_untf(buf,offset,name,shift):
	type = buf[offset:offset+4]
	[value] = struct.unpack('>d', buf[offset+4:offset+4+8])
	print shift*" ",name,"(UntF)",type,value
	return offset+12

def p_bool(buf,offset,name,shift):
	# ord converts 1 byte number
	print shift*" ",name,"(bool)",ord(buf[offset:offset+1])
	return offset+1

def p_doub(buf,offset,name,shift):
	# unpack 8 bytes ieee 754 value to floating point number
	[value] = struct.unpack('>d', buf[offset:offset+8])
	print shift*" ",name,"(doub)",value
	return offset+8

def p_enum(buf,offset,name,shift):
	[size1] = struct.unpack('>L', buf[offset:offset+4])
	offset+=4
	if size1 == 0:
		size1 = 4
	name1 = buf[offset:offset+size1]
	offset+=size1
	[size2] = struct.unpack('>L', buf[offset:offset+4])
	if size2 == 0:
		size2 = 4
	offset+=4
	name2 = buf[offset:offset+size2]
	offset+=size2
	print shift*" ",name,"(enum)",name1,name2
	return offset

def p_unkn(buf,offset,name,shift):
	# assume 4 bytes value
	# in such case offset+4:offset+8 is next length
	# and offset+8:offset+12 is next enum
	# check for it
	name = buf[offset+8:offset+12]
	if types.has_key(name):
		# everything is fine
		[size] = struct.unpack('>L',buf[offset:offset+4])
		return size,offset+4
	else:
		print "Failed with simple case\n"
		str_hex=""
		str_asc=""
		ml = 15
		for i in range(ml):
			try:
				str_hex+="%02x " % ord(buf[offset+i])
				if ord(buf[offset+i]) < 32 or 126<ord(buf[offset+i]):
					str_asc +='.'
				else:
					str_asc += buf[offset+i]
				print str_hex, str_asc
			except:
				print "Something failed"
		return str_hex+" "+str_asc,len(buf)+1

types = {"patt":p_patt,"desc":p_desc,"VlLs":p_vlls,"TEXT":p_text,"Objc":p_objc,\
					"UntF":p_untf,"bool":p_bool, "long":p_long, "doub":p_doub, "enum":p_enum, "tdta":p_tdta}

def parse_entry(buf,offset,shift):
	[nlen] = struct.unpack('>L',buf[offset:offset+4])
	if nlen == 0:
		nlen = 4
	offset = offset + 4
		
	name = buf[offset:offset+nlen]
	offset = offset + nlen
	type = buf[offset:offset+4]
	offset+=4
	if types.has_key(type):
		offset = types[type](buf,offset,name,shift)
	else:
		print "Unknown key:\t",name,type
		p_unkn(buf,offset,name,shift)
	return offset

def parse(buf):
	offset = 28
	shift = 0 # spaces from the left edge
	while offset < len(buf): 
		offset  = parse_entry(buf, offset, shift)

def main():
	if len(sys.argv) >= 2:
		filename = sys.argv[1]
		try:
			input = open(filename)
		except:
			print "No file"
			return 0
		buf = input.read()
		parse(buf)
			
	else:
		print "Use filename as an option"
		return

if __name__ == '__main__':
	main()
