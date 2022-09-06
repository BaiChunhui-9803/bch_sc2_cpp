#pragma once

#include "Mineral_TSP.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <random>

#define PopSize 30                  //��ʼ��Ⱥ��С
#define GenerationsMaxN 20000       //������
#define CountN 1000                 //���ƴ���
#define p_cross 0.7                      //�������
#define p_mutate 0.05                      //�������
#define MaxValue 200                //·���������ֵ
#define ValuetoFitness 10000        //·����������ֵ

struct CityPos {
    double x;
    double y;
};
double Hash[UnitSize];
CityPos City_Position[UnitSize];//����λ��
double City_Distance[UnitSize][UnitSize];//���о���ʵ�

class GA_TSP {
public:
    friend class Mineral_TSP;

private:
    int m_population[PopSize][UnitSize];    //��ʼ��Ⱥ
    double m_fitness[PopSize];              //��ʼ��Ⱥ�����Ӧֵ
    double m_distance[PopSize];             //ÿ���������·��
    int m_best_solution[UnitSize];          //�������Ž�
    double m_best_fitness;                  //�������Ž���Ӧֵ
    double m_best_distance;                 //�������Ž���·��
    int m_best_index;                       //��ʷ���Ž��±�
    double m_pop_best[GenerationsMaxN];     //������Ⱥ������·��
    double m_pop_avg[GenerationsMaxN];      //������Ⱥƽ����·��
    double m_dist_gbest[GenerationsMaxN];   //�������Ž���·������
    double dist_gbest[GenerationsMaxN];     //ȫ�����Ž�
    double gbest;                           //ȫ�����Ž�

    //��ֹ��Ԫid
    int m_begin1_id, m_begin2_id;
    std::vector<Node_TSP> m_city;
    //���Ž�->���
    std::vector<Node_TSP> m_best_city;

    //�������Ʊ���
    int m_count;

public:
    GA_TSP() = default;
    ~GA_TSP() = default;

    //������о���
    void calculate_City_Distance();
    //�㷨��ʼ��
    void initialize_Algorithm(const std::vector<Node_TSP>& vec);
    //�Ŵ��㷨���岿��
    std::vector<Node_TSP>& GA();
    //void GA();
    //��Ⱥ��ʼ��
    void initialize_population();
    //������Ӧֵ
    void calculate_fitness();
    //ѡ������
    void select();
    //��������
    void cross();
    //��������
    void mutate();
    //�����Ӧֵ
    double get_fitness(int a[UnitSize]);
    //���
    void output();
    //�����ͼ��Ϣ
    void output_draw();
    //������Ⱥ������·��
    double get_pop_best();
    //������Ⱥƽ����·��
    double get_pop_avg();



    //�������Ž⣬��city�����Ӧ
    void save_best_to_node();
};



void GA_TSP::initialize_Algorithm(const std::vector<Node_TSP>& vec) {

    //����Ϸ�����Ϣ�洢Ϊ�㷨������ϢCity_Position
    m_city.assign(vec.begin(), vec.end());
    for (int i = 0; i < UnitSize; i++) {
        City_Position[i].x = m_city.at(i).m_x;
        City_Position[i].y = m_city.at(i).m_y;
    }

    std::vector<int> vec_pop;

    for (int i = 0; i < UnitSize; i++) {
        vec_pop.push_back(i);
    }

    //�����ʼ�����ֹ��Begin��Blank��idΪm_begin_id��m_blank_id
    for (int i = 0; i < UnitSize; i++) {
        if (m_city.at(i).m_type == Begin) {
            m_begin1_id = m_city.at(i).m_mineral_id;
            std::swap(vec_pop.at(m_begin1_id), vec_pop.at(0));
        }
        if (m_city.at(i).m_type == Blank) {
            m_begin2_id = m_city.at(i).m_mineral_id;
            std::swap(vec_pop.at(m_begin2_id), vec_pop.at(UnitSize - 1));
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    for (int i = 0; i < PopSize; i++) {
        std::shuffle(vec_pop.begin() + 1, vec_pop.end() - 1, rng);
        for (int j = 0; j < UnitSize; j++) {
            m_population[i][j] = vec_pop.at(j);
        }
    }
    //std::cout << "�㷨��ʼ�����" << std::endl;
}

std::vector<Node_TSP>& GA_TSP::GA() {
    unsigned seed;
    seed = time(0);
    srand(seed);

    calculate_City_Distance();
    //��ʼ����Ⱥ
    initialize_population();
    //������Ӧֵ
    calculate_fitness();
    //����
    for (m_count = 0; m_count <= GenerationsMaxN; m_count++) {
        //ѡ��
        select();
        //����
        cross();
        //����
        mutate();
        calculate_fitness();

        m_pop_best[m_count] = get_pop_best();
        m_pop_avg[m_count] = get_pop_avg();
        m_dist_gbest[m_count] = m_best_distance;
        dist_gbest[m_count] = gbest;
    }
    output();
    output_draw();
    save_best_to_node();
    return m_best_city;

}

//������о���ʵ�City_Distance[i][j]
void GA_TSP::calculate_City_Distance() {
    int i, j;
    double temp1, temp2;
    for (i = 0; i < UnitSize; i++) {
        for (j = 0; j < UnitSize; j++) {
            temp1 = City_Position[j].x - City_Position[i].x;
            temp2 = City_Position[j].y - City_Position[i].y;
            City_Distance[i][j] = sqrt(temp1 * temp1 + temp2 * temp2);
        }
    }
}

//���鸴��
inline void copy(int a[], int b[]) {
    int i = 0;
    for (i = 0; i < UnitSize; i++) {
        a[i] = b[i];
    }
}

//��Ⱥ��ʼ��
void GA_TSP::initialize_population() {
    int i, j, r;
    gbest = MaxValue;
    m_best_distance = MaxValue;
    m_best_fitness = 0;//��ӦֵԽ��Խ��
}

//������Ӧֵ
void GA_TSP::calculate_fitness() {
    int i, j;
    int start, end;
    int best_index = 0;
    for (i = 0; i < PopSize; i++) {//��ÿ���������·������Ӧֵ
        m_distance[i] = 0;
        for (j = 1; j < UnitSize; j++) {
            start = m_population[i][j - 1]; end = m_population[i][j];
            m_distance[i] += City_Distance[start][end];//city.Distance[i]ÿ���������·��
        }
        m_fitness[i] = ValuetoFitness / m_distance[i];
        if (m_fitness[i] > m_fitness[best_index])//ѡ��������Ӧֵ����ѡ�����и����е����·��
            best_index = i;
    }

    copy(m_best_solution, m_population[best_index]);//�����Ÿ��忽����city.BestRooting
    m_best_fitness = m_fitness[best_index];
    m_best_distance = m_distance[best_index];
    m_best_index = best_index;
    if (m_best_distance < gbest)
        gbest = m_best_distance;

}

//ѡ������
void GA_TSP::select() {
    int TempColony[PopSize][UnitSize];
    int i, j, t;
    double s;
    double GaiLv[PopSize];
    double SelectP[PopSize + 1];
    double avg;
    double sum = 0;
    for (i = 0; i < PopSize; i++) {
        sum += m_fitness[i];
    }
    for (i = 0; i < PopSize; i++) {
        GaiLv[i] = m_fitness[i] / sum;
    }
    SelectP[0] = 0;
    for (i = 0; i < PopSize; i++) {
        SelectP[i + 1] = SelectP[i] + GaiLv[i] * RAND_MAX;
    }
    //void *memcpy(void *dest, const void *src, size_t n)��Դsrc��ָ���ڴ��ַ����ʼλ�ÿ�ʼ����n���ֽڵ�Ŀ��dest��ָ���ڴ��ַ����ʼλ����
    memcpy(TempColony[0], m_population[m_best_index], sizeof(TempColony[0]));
    for (t = 1; t < PopSize; t++) {
        double ran = rand() % RAND_MAX + 1;
        s = (double)ran / 100.0;
        for (i = 1; i < PopSize; i++) {
            if (SelectP[i] >= s)
                break;
        }
        memcpy(TempColony[t], m_population[i - 1], sizeof(TempColony[t]));
    }
    for (i = 0; i < PopSize; i++) {
        memcpy(m_population[i], TempColony[i], sizeof(TempColony[i]));
    }
}

//��������
void GA_TSP::cross() {
    int i, j, t, l;
    int a, b, ca, cb;
    int Temp1[UnitSize], Temp2[UnitSize];
    for (i = 0; i < PopSize; i++) {
        double s = ((double)(rand() % RAND_MAX)) / RAND_MAX;
        if (s < p_cross) {
            cb = rand() % PopSize;
            ca = cb;
            if (ca == m_best_index || cb == m_best_index)//�������������ֱ�ӽ����´�ѭ��
                continue;
            do {
                l = rand() % (UnitSize - 2) + 1;  //1-11
                a = rand() % (UnitSize - 2) + 1;  //1-20
            } while ((a + l) >= UnitSize);

            memset(Hash, 0, sizeof(Hash));//void *memset(void *s, int ch, size_t n);��s�е�ǰλ�ú����n���ֽ� �� ch �滻������ s ��
            Temp1[0] = m_begin1_id;
            Temp1[UnitSize - 1] = m_begin2_id;
            Hash[Temp1[0]] = 1;
            Hash[Temp1[UnitSize - 1]] = 1;
            for (j = 1; j <= l; j++) {//����˳�������ѡ������ͨ��Hash���Ϊ1
                Temp1[j] = m_population[cb][a + j - 1];
                Hash[Temp1[j]] = 1;
            }
            for (t = 1; t < (UnitSize - 1); t++) {
                if (Hash[m_population[ca][t]] == 0) {
                    Temp1[j] = m_population[ca][t];
                    Hash[m_population[ca][t]] = 1;
                    j++;
                }
            }
            memcpy(m_population[ca], Temp1, sizeof(Temp1));
        }
    }
}

//��������
void GA_TSP::mutate() {
    int i, k, m;
    int Temp[UnitSize];
    for (k = 0; k < PopSize; k++) {
        double s = ((double)(rand() % RAND_MAX)) / RAND_MAX;//�����������0~1��
        i = rand() % PopSize;//�������0~POPSIZE֮�����
        if (s < p_mutate && i != m_best_index) {//i!=city.BestNum������֤���ŵĸ��岻����
            int a, b, t;
            a = (rand() % (UnitSize - 2)) + 1;
            b = (rand() % (UnitSize - 2)) + 1;
            copy(Temp, m_population[i]);
            if (a > b) {//��֤��b>=a
                t = a;
                a = b;
                b = t;
            }
            for (m = a; m < (a + b) / 2; m++) {
                t = Temp[m];
                Temp[m] = Temp[a + b - m];
                Temp[a + b - m] = t;
            }
            if (get_fitness(Temp) < get_fitness(m_population[i])) {
                a = (rand() % (UnitSize - 2)) + 1;
                b = (rand() % (UnitSize - 2)) + 1;
                memcpy(Temp, m_population[i], sizeof(Temp));
                if (a > b) {
                    t = a;
                    a = b;
                    b = t;
                }
                for (m = a; m < (a + b) / 2; m++) {
                    t = Temp[m];
                    Temp[m] = Temp[a + b - m];
                    Temp[a + b - m] = t;
                }

                if (get_fitness(Temp) < get_fitness(m_population[i]))
                {
                    a = (rand() % (UnitSize - 2)) + 1;
                    b = (rand() % (UnitSize - 2)) + 1;
                    memcpy(Temp, m_population[i], sizeof(Temp));
                    if (a > b) {
                        t = a;
                        a = b;
                        b = t;
                    }
                    for (m = a; m < (a + b) / 2; m++) {
                        t = Temp[m];
                        Temp[m] = Temp[a + b - m];
                        Temp[a + b - m] = t;
                    }
                }
            }
            memcpy(m_population[i], Temp, sizeof(Temp));
        }
    }
}

//�����Ӧֵ
double GA_TSP::get_fitness(int a[UnitSize]) {

    int i, start, end;
    double Distance = 0;
    for (i = 0; i < (UnitSize - 1); i++) {
        start = a[i];
        end = a[i + 1];
        Distance += City_Distance[start][end];
    }
    return ValuetoFitness / Distance;
}

//���
void GA_TSP::output() {
    int i, j;
    std::cout << "���·��Ϊ:" << std::endl;
    for (i = 0; i < UnitSize; i++) {
        std::cout << m_best_solution[i] << ' ';
    }
    std::cout << std::endl;
    std::cout << "���·��ֵΪ:" << m_best_distance << std::endl;
}

//�����ͼ��Ϣ
void GA_TSP::output_draw() {
    std::ofstream fout;
    //��0�����������޳����������
    fout.open("D:/bch_sc2/project/pathing/datafile/pop_best.txt");
    for (int i = 1; i < GenerationsMaxN; i++) {
        fout << i << ' ' << m_pop_best[i] << std::endl;
    }
    fout.close();

    fout.open("D:/bch_sc2/project/pathing/datafile/pop_avg.txt");
    for (int i = 1; i < GenerationsMaxN; i++) {
        fout << i << ' ' << m_pop_avg[i] << std::endl;
    }
    fout.close();

    fout.open("D:/bch_sc2/project/pathing/datafile/dist_gbest.txt");
    for (int i = 1; i < GenerationsMaxN; i++) {
        fout << i << ' ' << dist_gbest[i] << std::endl;
    }
    fout.close();
}

inline double GA_TSP::get_pop_best() {
    double pop_best = 300;
    for (int i = 0; i < PopSize; i++) {
        if (m_distance[i] < pop_best)
            pop_best = m_distance[i];
    }
    return pop_best;
}

inline double GA_TSP::get_pop_avg() {
    double pop_avg = 0;
    double pop_sum = 0;

    for (int i = 0; i < PopSize; i++) {
        pop_sum += m_distance[i];
    }
    pop_avg = pop_sum / PopSize;
    return pop_avg;
}



void GA_TSP::save_best_to_node() {
    for (int i = 0; i < UnitSize; i++) {
        if (i == 0) {
            Node_TSP node(Begin, 0, 0, City_Position[m_best_solution[i]].x, City_Position[m_best_solution[i]].y, m_city.at(0).m_z);
            m_best_city.push_back(node);
        }
        else if (i > 0 && i < (UnitSize - 1)) {
            Node_TSP node(Mineral, i, i, City_Position[m_best_solution[i]].x, City_Position[m_best_solution[i]].y, m_city.at(0).m_z);
            m_best_city.push_back(node);
        }
        else if (i == (UnitSize - 1)) {
            Node_TSP node(Blank, UnitSize - 1, UnitSize - 1, City_Position[m_best_solution[i]].x, City_Position[m_best_solution[i]].y, m_city.at(0).m_z);
            m_best_city.push_back(node);
            /*std::cout << m_best_city.at(UnitSize - 1).m_mineral_id << std::endl;*/
        }
    }

    std::ofstream fout;
    fout.open("D:/bch_sc2/project/pathing/datafile/best_solution.txt");

    for (auto it : m_best_city) {
        //std::cout << it.m_type << '\t' << it.m_mineral_id << '\t' << it.m_mineral_code << '\t' << it.m_x << '\t' << it.m_y << std::endl;
        fout << it.m_x << '\t' << it.m_y << std::endl;
    }

    fout.close();
    //std::cout << "���Ž�������Ѿ������best.txt" << std::endl;
}
