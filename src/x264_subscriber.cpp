#include "x264_image_transport/x264_subscriber.h"
#include <sensor_msgs/image_encodings.h>
#include <boost/scoped_array.hpp>
#include <vector>


//using namespace std;
namespace x264_image_transport {
    
    namespace enc = sensor_msgs::image_encodings;


    x264Subscriber::x264Subscriber()
        :   latest_image_(new sensor_msgs::Image()), initialized_(false)
    {	
	n = 2;
	/*for(int i=0; i<n; i++){
        //m_pFormatCtx = NULL;
		codecs[i].m_pCodecCtx = NULL;
		codecs[i].m_pCodec = NULL;
		codecs[i].m_pFrame = NULL;
		codecs[i].m_pFrameRGB = NULL;
		codecs[i].m_img_convert_ctx = NULL;
		codecs[i].m_inputFormat = NULL;
		codecs[i].m_buffer = NULL;
	}*/
		m_pCodecCtx = NULL;
		m_pCodec = NULL;
		m_pFrame = NULL;
		m_pFrameRGB = NULL;
		m_img_convert_ctx = NULL;
		m_inputFormat = NULL;
		m_buffer = NULL;
	codecs = NULL;
    }

    void x264Subscriber::initialize_codec(int width, int height)
    {
        ROS_INFO("x264Subscriber::initialize_codec(int width = %i, int height = %i)",width, height);
	codecs = (codec_meta *) malloc(n*sizeof(codec_meta));
	// must be called before using avcodec lib
        avcodec_register_all();
        //Register codecs, devices and formats
        av_register_all();
        //Initialize network, this is new from april 2013, it will initialize the RTP
        avformat_network_init();

        //HARDCODING IMAGE SIZE
        latest_image_->height = height;
        latest_image_->width = width;
        latest_image_->step = latest_image_->width*3;
        latest_image_->data.resize(latest_image_->step * latest_image_->height);
        latest_image_->encoding = enc::RGB8;



        //Setting up codec information

        //------------------------------------------------------------------------------
        //Set the codec to H264
        ROS_INFO("VideoOutputContext::initialize : Setting AVCodecs");
	codecs[0].m_pCodec = avcodec_find_decoder(CODEC_ID_H264);
	codecs[1].m_pCodec = avcodec_find_decoder(CODEC_ID_MPEG4);
	for(int i=0; i<n;i++){
		if(codecs[i].m_pCodec==NULL) {
		    ROS_ERROR("VideoOutputContext::initialize : Unsupported codec");
		    return;
		}

		//Set the codec Context
		ROS_INFO("VideoOutputContext::initialize : Setting AVCodecContext");
		codecs[i].m_pCodecCtx = avcodec_alloc_context3(codecs[i].m_pCodec);
		codecs[i].m_pCodecCtx->width = latest_image_->width;
		codecs[i].m_pCodecCtx->height= latest_image_->height;

		ROS_INFO("Opening Codec %d", i);

		// open the decoder codec 
		if (avcodec_open2(codecs[i].m_pCodecCtx, codecs[i].m_pCodec, NULL) < 0)
		{
		    ROS_ERROR("Could not open the decoder");
		    return;
		}

		ROS_INFO("Allocating frame %d", i);
		codecs[i].m_pFrame=av_frame_alloc();

		// Allocate an AVFrame structure
		codecs[i].m_pFrameRGB=av_frame_alloc();
		if(codecs[i].m_pFrameRGB==NULL)
		{
		    ROS_ERROR("Cannot allocate frame");
		    return;
		}
		
		ROS_INFO("Making buffer %d", i);
		// Determine required buffer size and allocate buffer
		int numBytes=avpicture_get_size(PIX_FMT_RGB24, codecs[i].m_pCodecCtx->width,codecs[i].m_pCodecCtx->height);
		codecs[i].m_buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

		// Assign appropriate parts of buffer to image planes in pFrameRGB
		// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
		// of AVPicture
		avpicture_fill((AVPicture *)codecs[i].m_pFrameRGB, codecs[i].m_buffer, PIX_FMT_RGB24,
		               codecs[i].m_pCodecCtx->width, codecs[i].m_pCodecCtx->height);
		codecs[i].m_img_convert_ctx = NULL;
	}

/*
        m_pCodec=avcodec_find_decoder(CODEC_ID_H264);

        if(m_pCodec==NULL) {
            ROS_ERROR("VideoOutputContext::initialize : Unsupported codec");
            return;
        }

        //Set the codec Context
        ROS_INFO("VideoOutputContext::initialize : Setting AVCodecContext");
        m_pCodecCtx = avcodec_alloc_context3(m_pCodec);


        
        m_pCodecCtx->width = latest_image_->width;
        m_pCodecCtx->height= latest_image_->height;

 // Open codec
        ROS_INFO("Opening Codec");

        /* open the decoder codec 
        if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0)
        {
            ROS_ERROR("Could not open the decoder");
            return;
        }



        m_pFrame=avcodec_alloc_frame();

        // Allocate an AVFrame structure
        m_pFrameRGB=avcodec_alloc_frame();
        if(m_pFrameRGB==NULL)
        {
            ROS_ERROR("Cannot allocate frame");
            return;
        }

        // Determine required buffer size and allocate buffer
        int numBytes=avpicture_get_size(PIX_FMT_RGB24, m_pCodecCtx->width,m_pCodecCtx->height);
        m_buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

        // Assign appropriate parts of buffer to image planes in pFrameRGB
        // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
        // of AVPicture
        avpicture_fill((AVPicture *)m_pFrameRGB, m_buffer, PIX_FMT_RGB24,
                       m_pCodecCtx->width, m_pCodecCtx->height);

	ROS_INFO("SWScale %d,%d,%d", m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt);
	/* */
        initialized_ = true;
        ROS_INFO("x264Subscriber::initialize_codec() Codec Ready!");
    }

    x264Subscriber::~x264Subscriber()
    {
        initialized_ = false;
        for(int i=0;i<n;i++){
		if (codecs[i].m_pCodecCtx)
		{
		    avcodec_close(codecs[i].m_pCodecCtx);
		}
		
		if(codecs[i].m_pFrame)
		{
		     av_free(codecs[i].m_pFrame);
		}
		
		if (codecs[i].m_pFrameRGB)
		{
		    av_free(codecs[i].m_pFrameRGB);
		}
		    
		if (codecs[i].m_img_convert_ctx)
		{
		    sws_freeContext(codecs[i].m_img_convert_ctx);
		}
	}
	free(codecs);
    }

    void x264Subscriber::subscribeImpl(ros::NodeHandle &nh, const std::string &base_topic, uint32_t queue_size,
                                       const Callback &callback, const ros::VoidPtr &tracked_object,
                                       const image_transport::TransportHints &transport_hints)
    {
        //TODO UNDERSTAND THOSE PARAMETERS...


        // queue_size doesn't account for the 3 header packets, so we correct (with a little extra) here.
        queue_size += 4;
        typedef image_transport::SimpleSubscriberPlugin<x264_image_transport::x264Packet> Base;
        Base::subscribeImpl(nh, base_topic, queue_size, callback, tracked_object, transport_hints);

        // Set up reconfigure server for this topic
#ifndef __APPLE__       
        reconfigure_server_ = boost::make_shared<ReconfigureServer>(this->nh());
        ReconfigureServer::CallbackType f = boost::bind(&x264Subscriber::configCb, this, _1, _2);
        reconfigure_server_->setCallback(f);
#endif        
    }

#ifndef __APPLE__
    void x264Subscriber::configCb(Config& config, uint32_t level)
    {
        //TODO HANDLE CONFIGURATION
        ROS_INFO("x264Subscriber::configCb not implemented yet");
    }
#endif

    void x264Subscriber::convert_rgb(AVCodecContext *codec, AVFrame *inFrame, AVFrame *outFrame)
    {
            //Initialize converter context if required
	    	
            if (!m_img_convert_ctx)
            {
		   ROS_INFO("SWScale %d,%d,%d", codec->width, codec->height, codec->pix_fmt);
                   m_img_convert_ctx =sws_getContext(codec->width, codec->height,codec->pix_fmt, //src
                                                      codec->width, codec->height, PIX_FMT_RGB24, //dest
                                                      SWS_FAST_BILINEAR,NULL, NULL, NULL);
            }

            //convert to RGB
            sws_scale(m_img_convert_ctx, inFrame->data,
                              inFrame->linesize, 0,
                              codec->height,
                              outFrame->data, outFrame->linesize);
		

    }

    void x264Subscriber::internalCallback(const x264_image_transport::x264PacketConstPtr& message, const Callback& callback)
    {
        //ROS_INFO("x264Subscriber::internalCallback");
        
        if (!initialized_)
        {
            initialize_codec(message->img_width, message->img_height);
        }
        
        //Something went wrong
        if (!initialized_)
            return;

	if(message->codec < 0 || message->codec >= n){
		ROS_INFO("Unsupported Codec! Can not decode!");
		return;
	}

	int index = message->codec;
	//int index = 1;
        //decode video...

        //Create packet
        AVPacket packet;
        av_init_packet(&packet);

        //allocate packet memory
        av_new_packet(&packet,message->data.size());

        //copy data
        memcpy(packet.data,&message->data[0],message->data.size());
	/* 
	int frameFinished;
	int result = avcodec_decode_video2(m_pCodecCtx, m_pFrame, &frameFinished,&packet);


        if(result >= 0 && frameFinished > 0)
        {
              //ROS_INFO("Decoding result : %i frameFinished : %i",result,frameFinished);
              
              //Convert input image to RGB32 format
              convert_rgb(m_pCodecCtx,m_pFrame,m_pFrameRGB);

              //Copy data (RGB24)
              memcpy(&latest_image_->data[0],m_buffer,m_pCodecCtx->width *m_pCodecCtx->height * 3);

              //Affect header
              latest_image_->header = message->header;

              //Call callback (new image received)
              callback(latest_image_);
        }
	 */
        //try decoding...
        int frameFinished = 0;
        int result = avcodec_decode_video2(codecs[index].m_pCodecCtx, codecs[index].m_pFrame, &frameFinished,&packet);

        if(result >= 0 && frameFinished > 0)
        {
              //ROS_INFO("Decoding result : %i frameFinished : %i",result,frameFinished);
              
              //Convert input image to RGB24 format
              //convert_rgb(m_pCodecCtx,m_pFrame,m_pFrameRGB);
	      if(!codecs[index].m_img_convert_ctx){
 			codecs[index].m_img_convert_ctx =sws_getContext(codecs[index].m_pCodecCtx->width, 
							codecs[index].m_pCodecCtx->height,
							codecs[index].m_pCodecCtx->pix_fmt,
                                                      	codecs[index].m_pCodecCtx->width, 
							codecs[index].m_pCodecCtx->height, 
							PIX_FMT_RGB24, SWS_FAST_BILINEAR,NULL, NULL, NULL);
	      }
	      sws_scale(codecs[index].m_img_convert_ctx, codecs[index].m_pFrame->data, codecs[index].m_pFrame->linesize, 0,
                              codecs[index].m_pCodecCtx->height, codecs[index].m_pFrameRGB->data, codecs[index].m_pFrameRGB->linesize);
              //Copy data (RGB24)
              memcpy(&latest_image_->data[0],codecs[index].m_buffer,codecs[index].m_pCodecCtx->width * codecs[index].m_pCodecCtx->height * 3);

              //Affect header
              latest_image_->header = message->header;

              //Call callback (new image received)
              callback(latest_image_);
        }
	
        //free packet
        av_free_packet(&packet);
    }


} //namespace x264_image_transport
