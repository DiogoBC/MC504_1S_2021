#include "projeto1.h"

picture * open_picture (char * picture_name){

    FIBITMAP * bitmap_in;
    RGBQUAD colour;
    uint32_t width, height, area, idx;
    picture * pic_p = NULL;

    bitmap_in = FreeImage_Load (FIF_JPEG, picture_name, 0);

    if (bitmap_in){

        pic_p = (picture *) malloc (1 * sizeof (picture));

        pic_p->width = (uint32_t) FreeImage_GetWidth (bitmap_in);
        pic_p->height = (uint32_t) FreeImage_GetHeight (bitmap_in);
        area = pic_p->width * pic_p->height;
        pic_p->red_channel = (float *) malloc (area * sizeof (float));
        pic_p->green_channel = (float *) malloc (area * sizeof (float));
        pic_p->blue_channel = (float *) malloc (area * sizeof (float));

        for (width=0; width<pic_p->width; width++){
            for (height=0; height<pic_p->height; height++){                
                FreeImage_GetPixelColor (bitmap_in, width, height, &colour);                
                idx = width + (height * pic_p->width);
                pic_p->red_channel[idx] = (float) colour.rgbRed;
                pic_p->green_channel[idx] = (float) colour.rgbGreen;
                pic_p->blue_channel[idx] = (float) colour.rgbBlue;
            }
        }
    }

    return (pic_p);
}

void free_picture (picture * pic_p){

    free (pic_p->red_channel);
    free (pic_p->green_channel);
    free (pic_p->blue_channel);
    free (pic_p);
}

void save_picture (char * file_name, picture * pic_p){

    FIBITMAP *bitmap_out;
    RGBQUAD colour;
    uint32_t width, height, idx;

    bitmap_out = FreeImage_Allocate (pic_p->width, pic_p->height, 24, 0, 0, 0);

    for (width=0; width<pic_p->width; width++){
        for (height=0; height<pic_p->height; height++){
            idx = width + (height * pic_p->width);
            colour.rgbRed = (float) pic_p->red_channel[idx];
            colour.rgbGreen = (float) pic_p->green_channel[idx];
            colour.rgbBlue = (float) pic_p->blue_channel[idx];
            FreeImage_SetPixelColor (bitmap_out, width, height, &colour);
        }
    }

    FreeImage_Save (FIF_JPEG, bitmap_out, file_name, JPEG_DEFAULT);
}

picture * copy_picture (picture * pic_p){

    uint32_t width, height, idx, area;
    picture * new_pic_p = NULL;

    if (pic_p != NULL){
        new_pic_p = (picture *) malloc (1 * sizeof (picture));
        new_pic_p->width = pic_p->width;
        new_pic_p->height = pic_p->height;
        area = new_pic_p->width * new_pic_p->height;
        new_pic_p->red_channel = (float *) malloc (area * sizeof (float));
        new_pic_p->green_channel = (float *) malloc (area * sizeof (float));
        new_pic_p->blue_channel = (float *) malloc (area * sizeof (float));

        for (width=0; width<new_pic_p->width; width++){
            for (height=0; height<new_pic_p->height; height++){
                idx = width + (height * new_pic_p->width);
                new_pic_p->red_channel[idx] = pic_p->red_channel[idx];
                new_pic_p->green_channel[idx] = pic_p->green_channel[idx];
                new_pic_p->blue_channel[idx] = pic_p->blue_channel[idx];
            }
        }
    }

    return (new_pic_p);
}

void set_brightness (picture * pic_p, float factor){

    uint32_t width, height, idx;
    struct timeval rt0, rt1, drt;

    gettimeofday (&rt0, NULL);

    for (width=0; width<pic_p->width; width++){
        for (height=0; height<pic_p->height; height++){
            idx = width + (height * pic_p->width);
            pic_p->red_channel[idx] = CAP (pic_p->red_channel[idx] * factor);
            pic_p->green_channel[idx] = CAP (pic_p->green_channel[idx] * factor);
            pic_p->blue_channel[idx] = CAP (pic_p->blue_channel[idx] * factor);
        }
    }

    gettimeofday (&rt1, NULL);
    timersub (&rt1, &rt0, &drt);
    printf ("Tempo de processamento sem thread: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
}

void * set_colour_channel (void * arg){

    thread_arg * t_arg_p = (thread_arg *) arg;
    uint32_t width, height, idx;

    for (width=0; width<t_arg_p->pic_p->width; width++){
        for (height=0; height<t_arg_p->pic_p->height; height++){
            idx = width + (height * t_arg_p->pic_p->width);
            switch (t_arg_p->channel_sel){
                case RED:
                    t_arg_p->pic_p->red_channel[idx] = CAP (t_arg_p->pic_p->red_channel[idx] * t_arg_p->factor);
                    break;

                case GREEN:
                    t_arg_p->pic_p->green_channel[idx] = CAP (t_arg_p->pic_p->green_channel[idx] * t_arg_p->factor);
                    break;

                case BLUE:
                    t_arg_p->pic_p->blue_channel[idx] = CAP (t_arg_p->pic_p->blue_channel[idx] * t_arg_p->factor);
                    break;
            }
        }
    }

    return (0);
}

void set_brightness_with_threads (picture * pic_p, float factor){

    pthread_t threads[3];
    thread_arg t_args[3];
    uint8_t colour;
    struct timeval rt0, rt1, drt;

    gettimeofday (&rt0, NULL);

    for (colour=RED; colour<=BLUE; colour++){
        t_args[colour].channel_sel = colour;
        t_args[colour].factor = factor;
        t_args[colour].pic_p = pic_p;
        pthread_create (&(threads[colour]), NULL, set_colour_channel, (void *) &(t_args[colour]));
    }

    for (colour=RED; colour<=BLUE; colour++){
        pthread_join (threads[colour], NULL);
    }

    gettimeofday (&rt1, NULL);
    timersub (&rt1, &rt0, &drt);
    printf ("Tempo de processamento com thread: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
}

int main (){

    char input_file_name[] = "input.jpg";
    char output_file_name_no_thread[] = "output_no_thread.jpg";
    char output_file_name_thread[] = "output_thread.jpg";
    picture * pic_data_p, * copy_pic_data_p;
    float factor = 0.3;

    pic_data_p = open_picture (input_file_name);
    copy_pic_data_p = copy_picture (pic_data_p);
    set_brightness (pic_data_p, factor);
    set_brightness_with_threads (copy_pic_data_p, factor);
    save_picture (output_file_name_no_thread, pic_data_p);
    save_picture (output_file_name_thread, copy_pic_data_p);
    free_picture (pic_data_p);
    free_picture (copy_pic_data_p);

    return (0);
}