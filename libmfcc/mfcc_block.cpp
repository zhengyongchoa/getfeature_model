#include "mfcc_block.h"


double mfcc_block(std::vector<float >(samples) , int SAMPLE_COUNT, double **feature_vector ){
    int stride = 512;
    int length_frame = 1024;
    int length_DFT = 1024;
    int number_coefficients = 12;
    int number_filterbanks = 16;

    int number_feature_vectors;

    double pi = 3.14159265358979323846;


    // 采样率
    int SAMPLE_RATE = 16000;

    // 输出的结果变量
//    double **feature_vector;
    number_feature_vectors = (SAMPLE_COUNT - length_frame) / stride + 1;
//    feature_vector = new double*[number_feature_vectors];

    for (int i = 0; i < number_feature_vectors; i++){
        feature_vector[i] = new double[3 * number_coefficients];
    }

    // MFCC
    for (int i = 0; i <= SAMPLE_COUNT - length_frame; i += stride){
        double *frame = new double[length_frame];

        // pre-emphasis
        for (int j = 0; j < length_frame; j++){
            if (i + j < SAMPLE_COUNT){
                frame[j] = samples[i + j] - 0.95 * samples[i + j - 1];
            }
            else{
                frame[j] = 0;
            }
        }

        // windowing
        for (int j = 0; j < length_frame; j++){
            frame[j] *= 0.54 - 0.46 * cos(2 * pi * j / (length_frame - 1));
        }

        MFCC(length_frame, length_DFT, number_coefficients, number_filterbanks, SAMPLE_RATE, frame, feature_vector[i / stride]);

        delete[] frame;
    }

    // deltas
    for (int i = 0; i < number_feature_vectors; i++){
        int prev = (i == 0) ? (0) : (i - 1);
        int next = (i == number_feature_vectors - 1) ? (number_feature_vectors - 1) : (i + 1);

        for (int j = 0; j < number_coefficients; j++){
            feature_vector[i][number_coefficients + j] = (feature_vector[next][j] - feature_vector[prev][j]) / 2;
        }
    }

    // delta-deltas
    for (int i = 0; i < number_feature_vectors; i++){
        int prev = (i == 0) ? (0) : (i - 1);
        int next = (i == number_feature_vectors - 1) ? (number_feature_vectors - 1) : (i + 1);

        for (int j = number_coefficients; j < 2 * number_coefficients; j++){
            feature_vector[i][number_coefficients + j] = (feature_vector[next][j] - feature_vector[prev][j]) / 2;
        }
    }

//    FILE *file = fopen("./MFCC.txt", "wt");
//
//    for (int i = 0; i < number_feature_vectors; i++){
//        for (int j = 0; j < 3 * number_coefficients; j++){
//            fprintf(file, "%lf ", feature_vector[i][j]);
//        }
//        fprintf(file, "\n");
//    }
//    fclose(file);
//
//    for (int i = 0; i < number_feature_vectors; i++){
//        delete[] feature_vector[i];
//    }
//    delete[] feature_vector;

    return 0;
}
