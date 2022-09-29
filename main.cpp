#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstring>
#include "libpitch/libpyincpp.h"
#include "libmfcc/mfcc_block.h"
#include "libwav/wavreader.h"

using namespace std;
string split(string str,int ind);

string split(string str, int ind) {
    int size = str.size();
    vector<string> ans;
    int j = 0;
    for (int i = 0; i < size; i++) {
        if (str[i] == '/')
            ans.push_back(str.substr(j));
        j = i+1;
    }

    string  na = ans.at(ans.size()-ind);
    na.erase(0, 1); //删除第一个字符
//    for (string name : ans) {
//        cout<<name<<endl;
//    }
    return na;
}


int Get_FeatureSet(string &path ,  string &outpath){

    string audioname = split(path,1);
    string label = split(path,2);

    void* h_x = wav_read_open(path.c_str());

    int format, channels, sr, bits_per_sample;
    unsigned int data_length;
    int res = wav_get_header(h_x, &format, &channels, &sr, &bits_per_sample, &data_length);
    if (!res)
    {
        cerr << "get ref header error: " << res << endl;
        return -1;
    }
    int len = data_length * 8 / bits_per_sample; // audio  data  length
    std::vector<int16_t> tmp(len);
    res = wav_read_data(h_x, reinterpret_cast<unsigned char*>(tmp.data()), data_length);
    wav_read_close(h_x);
    if (res < 0)
    {
        cerr << "read wav file error: " << res << endl;
        return -1;
    }
    std::vector<float> x(len);
    for (int i = 0; i < tmp.size(); ++i) {
        x[i] = (float)tmp[i] /32767.f;
    }

    int SAMPLE_COUNT = 2*sr;
    vector<float>in(SAMPLE_COUNT);
    in.assign(x.begin()+sr, x.begin()+sr+SAMPLE_COUNT); //截取出SAMPLE_COUNT个点,从第 sr 点开始

    int BLOCK_SIZE = 1024;
    int STEP_SIZE = 512;
    int number_feature_vectors = (SAMPLE_COUNT - BLOCK_SIZE) / STEP_SIZE + 1;

    PyinCpp my_pyin(sr, BLOCK_SIZE, STEP_SIZE);
    std::vector<float> pitches = my_pyin.feed(in);
    // 打印 F0
//    for (const float pitch : pitches)
//    {
//        std::cout << pitch << " ";
//    }

    // 生成 MFCC ,注意： 子函数中重新定义number_feature_vectors的数值
    int number_coefficients = 12;
    double **feature_vector= new double *[number_feature_vectors] ;
    feature_vector[0] = new double[number_feature_vectors * number_coefficients * 3];
    mfcc_block( in,  SAMPLE_COUNT,  feature_vector);


    //  F0 + MFCC
    double fea[number_feature_vectors][number_coefficients*3+1];
    for (int k = 0; k < number_feature_vectors; ++k) {

        if (pitches[k]>50.0 && pitches[k] <400.0){
            fea[k][0] = pitches[k];
            for (int i = 0; i < number_coefficients*3; ++i) {
                fea[k][i+1] = feature_vector[k][i];
            }
        }else{
            fea[k][0]= 0;
            for (int i = 0; i < number_coefficients*3+1; ++i) {
                fea[k][i+1] = feature_vector[k][i];
            }
        }

    }

//    // generate MFCC data.txt
//    FILE *file = fopen("./MFCC.txt", "wt");
//    for (int i = 0; i < number_feature_vectors; i++){
//        for (int j = 0; j < 3 * number_coefficients; j++){
//            fprintf(file, "%lf ", feature_vector[i][j]);
//        }
//        fprintf(file, "\n");
//    }
//    fclose(file);

    for (int i = 0; i < number_feature_vectors; i++){
        delete[] feature_vector[i];
    }
    delete[] feature_vector;

    // generate Feature data.txt
    string outname = outpath + '/' ;
    outname.append(label,0,1);
    audioname =  "_" + audioname+ ".txt";
    outname = outname +audioname;
//    cout<<endl<< outname<<endl;

    FILE *file1 = fopen(outname.c_str(), "w");
    if (file1 == NULL){
        printf("Failed to save data!");
        exit(1);
    }
    for (int i = 0; i < number_feature_vectors; i++){
        for (int j = 0; j < 3 * number_coefficients+1 ; j++){
            fprintf(file1, "%lf ", fea[i][j]);
        }
        fprintf(file1, "\n");
    }
    fclose(file1);

    return 0;
}


int main() {
    string outpath = "/Users/zyc/code/C/getfeature_model/feature";
    ifstream inFile;
    inFile.open("/Users/zyc/code/C/getfeature_model/train_list.txt");
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop
    }
    string line;
    vector<string> lines;
    while(getline(inFile, line))    //获取每一行数据
    {
        lines.push_back(line);   //将每一行依次存入到vector中
        cout << line << endl;
//        cout<<endl<<typeid(line).name()<<endl;
        Get_FeatureSet(line, outpath);

    }
    inFile.close();

}
