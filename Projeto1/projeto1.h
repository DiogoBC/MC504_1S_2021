#ifndef PROJETO1_H
    #define PROJETO1_H

    #define _DEFAULT_SOURCE

    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include <FreeImage.h>
    #include <sys/time.h>
    #include <pthread.h>

    #define CAP(x) (x > 255.0 ? 255.0 : x)

    typedef enum _pic_channel {

        RED,
        GREEN,
        BLUE

    } pic_channel;

    typedef struct _picture {

        uint32_t width, height;
        float * red_channel,  * green_channel, * blue_channel;

    } picture;

    typedef struct _thread_arg {

        picture * pic_p;
        pic_channel channel_sel;
        float factor;

    } thread_arg;

    /**
     * @brief Abre imagem .jpg e salva matrizes dos canais RGB e dimensões
     * da imagem na struct picture
     * @param picture_name Nome do arquivo .jpg
     * @return Ponteiro para a struct picture
     */
    picture * open_picture (char * picture_name);

    /**
     * @brief Libera memória alocada para dados da imagem
     * @param pic_p Ponteiro para struct picture
     */
    void free_picture (picture * pic_p);

    /**
     * @brief Copia dados da struct picture em uma nova struct
     * @param pic_p Ponteiro para struct picture a ser copiada
     * @return Ponteiro para nova struct picture
     */
    picture * copy_picture (picture * pic_p);

    /**
     * @brief Multiplica os pixels dos canais RGB da imagem
     * @param pic_p Ponteiro para struct picture com os dados da imagem
     * @param factor Fator multiplicador dos valores dos pixels da imagem
     */
    void set_brightness (picture * pic_p, float factor);

    /**
     * @brief Função utilizada pela thread para multiplicar os valores dos pixels de um canal de cor
     * @param arg Ponteiro para struct de argumento para thread
     */
    void * set_colour_channel (void * arg);

    /**
     * @brief Multiplica os pixels dos canais RGB da imagem utilizando threads
     * @param pic_p Ponteiro para struct picture com os dados da imagem
     * @param factor Fator multiplicador dos valores dos pixels da imagem
     */
    void set_brightness_with_threads (picture * pic_p, float factor);

#endif