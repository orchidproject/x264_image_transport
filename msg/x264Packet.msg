Header header     # Original sensor_msgs/Image header
uint8[] data      # x264 NAL unit
int32 img_width   # Image width
int32 img_height  # Image height
uint8 codec	  # Specifies the exact codec used by the encoder

uint8 CODEC_H264 = 0
uint8 CODEC_MPEG4 = 1

