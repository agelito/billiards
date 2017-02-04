// tga_loader.c

#define TGA_DATA_BITMASK 0x7
#define TGA_RLE_BITMASK  0x8

#define TGA_RLE_PACKET_BITMASK 0x80
#define TGA_RLE_PACKET_SHIFT 7

typedef enum tga_data_type
{
    TGA_DATA_NONE = 0,
    TGA_DATA_COLOR_MAPPED = 1,
    TGA_DATA_RGB = 2,
    TGA_DATA_BW = 3,
} tga_data_type;

#pragma pack(push, 1)
typedef struct tga_header
{
    unsigned char id_length;
    unsigned char color_map_type;
    unsigned char data_type_code;
    unsigned short color_map_origin;
    unsigned short color_map_length;
    unsigned char color_map_depth;
    unsigned short origin_x;
    unsigned short origin_y;
    unsigned short width;
    unsigned short height;
    unsigned char bits_per_pixel;
    unsigned char image_descriptor;
} tga_header;
#pragma pack(pop)

static int
tga_decode_5551(unsigned char* input_bytes, unsigned char* output_bytes)
{
    unsigned char low = input_bytes[0];
    unsigned char high = input_bytes[1];
    unsigned short pixel_in = (high << 8 | low);

    output_bytes[0] = (pixel_in >> 15) * 255;
    output_bytes[1] = ((pixel_in >> 10) & 0x1f) << 3;
    output_bytes[2] = ((pixel_in >> 5) & 0x1f) << 3;
    output_bytes[3] = ((pixel_in >> 0) & 0x1f) << 3;
    return 4;
}

static int
tga_decode_8888(unsigned char* input_bytes, unsigned char* output_bytes)
{
    output_bytes[0] = (input_bytes[2]);
    output_bytes[1] = (input_bytes[1]);
    output_bytes[2] = (input_bytes[0]);
    output_bytes[3] = (input_bytes[3]);
    return 4;
}

static int
tga_decode_888(unsigned char* input_bytes, unsigned char* output_bytes)
{
    output_bytes[0] = (input_bytes[2]);
    output_bytes[1] = (input_bytes[1]);
    output_bytes[2] = (input_bytes[0]);
    return 3;
}

static int
tga_decode_8(unsigned char* input_bytes, unsigned char* output_bytes)
{
    output_bytes[0] = (input_bytes[0]);
    output_bytes[1] = (input_bytes[0]);
    output_bytes[2] = (input_bytes[0]);
    return 3;
}

typedef int (*tga_decode)(unsigned char*, unsigned char*);

static void
tga_decode_image(tga_header* header, unsigned char* input_bytes, unsigned char* output_bytes)
{
    tga_decode decoder = 0;
    unsigned int in_pixel_stride = 0;
    switch(header->bits_per_pixel)
    {
    case 8:
	decoder = tga_decode_8;
	in_pixel_stride = 1;
	break;
    case 16:
	decoder = tga_decode_5551;
	in_pixel_stride = 2;
	break;
    case 24:
	decoder = tga_decode_888;
	in_pixel_stride = 3;
	break;
    case 32:
	decoder = tga_decode_8888;
	in_pixel_stride = 4;
	break;
    }

    assert(decoder != 0);

    int use_rle = (header->data_type_code & TGA_RLE_BITMASK) == TGA_RLE_BITMASK;

    unsigned char* read_at = input_bytes;
    unsigned char* out_at = output_bytes;
    
    unsigned int pixel = 0;
    unsigned int pixel_count = (header->width * header->height);
    
    if(use_rle)
    {
	while(pixel < pixel_count)
	{
	    unsigned char rle_header = *(read_at++);
	    unsigned char rle_count = (rle_header & 0x7f);
	    unsigned char rle_packet = (rle_header & TGA_RLE_PACKET_BITMASK) >> TGA_RLE_PACKET_SHIFT;

	    int decoded_bytes = decoder(read_at, out_at);
	    out_at += decoded_bytes;
	    read_at += in_pixel_stride;
	    pixel++;
	    
	    if(rle_packet)
	    {
		unsigned char* copy_from = (out_at - decoded_bytes);

		int i;
		for_range(i, rle_count)
		{
		    memcpy(out_at, copy_from, decoded_bytes);
		    out_at += decoded_bytes;
		}

		pixel += rle_count;
	    }
	    else
	    {
		int i;
		for_range(i, rle_count)
		{
		    decoded_bytes = decoder(read_at, out_at);
		    out_at += decoded_bytes;
		    read_at += in_pixel_stride;
		}

		pixel += rle_count;
	    }
	}

	assert(pixel == pixel_count);

    }
    else
    {
	for_range(pixel, pixel_count)
	{
	    int decoded_bytes = decoder(read_at, out_at);
	    read_at += in_pixel_stride;
	    out_at += decoded_bytes;
	}

	assert(pixel == pixel_count);
    }
}
