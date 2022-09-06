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

#define PopSize 30                  //初始种群大小
#define GenerationsMaxN 20000       //最大代数
#define CountN 1000                 //控制代数
#define p_cross 0.7                      //交叉概率
#define p_mutate 0.05                      //变异概率
#define MaxValue 200                //路径上限最大值
#define ValuetoFitness 10000        //路径长度修正值

struct CityPos {
    double x;
    double y;
};
double Hash[UnitSize];
CityPos City_Position[UnitSize];//城市位置
double City_Distance[UnitSize][UnitSize];//城市距离词典

class GA_TSP {
public:
    friend class Mineral_TSP;

private:
    int m_population[PopSize][UnitSize];    //初始种群
    double m_fitness[PopSize];              //初始种群解的适应值
    double m_distance[PopSize];             //每个个体的总路径
    int m_best_solution[UnitSize];          //历代最优解
    double m_best_fitness;                  //历代最优解适应值
    double m_best_distance;                 //历代最优解总路径
    int m_best_index;                       //历史最优解下标
    double m_pop_best[GenerationsMaxN];     //历代种群最优总路径
    double m_pop_avg[GenerationsMaxN];      //历代种群平均总路径
    double m_dist_gbest[GenerationsMaxN];   //历代最优解总路径数组
    double dist_gbest[GenerationsMaxN];     //全局最优解
    double gbest;                           //全局最优解

    //起止单元id
    int m_begin1_id, m_begin2_id;
    std::vector<Node_TSP> m_city;
    //最优解->结点
    std::vector<Node_TSP> m_best_city;

    //迭代控制变量
    int m_count;

public:
    GA_TSP() = default;
    ~GA_TSP() = default;

    //计算城市距离
    void calculate_City_Distance();
    //算法初始化
    void initialize_Algorithm(const std::vector<Node_TSP>& vec);
    //遗传算法主体部分
    std::vector<Node_TSP>& GA();
    //void GA();
    //种群初始化
    void initialize_population();
    //计算适应值
    void calculate_fitness();
    //选择算子
    void select();
    //交叉算子
    void cross();
    //变异算子
    void mutate();
    //获得适应值
    double get_fitness(int a[UnitSize]);
    //输出
    void output();
    //输出画图信息
    void output_draw();
    //历代种群最优总路径
    double get_pop_best();
    //历代种群平均总路径
    double get_pop_avg();



    //保存最优解，与city坐标对应
    void save_best_to_node();
};



void GA_TSP::initialize_Algorithm(const std::vector<Node_TSP>& vec) {

    //将游戏结点信息存储为算法可用信息City_Position
    m_city.assign(vec.begin(), vec.end());
    for (int i = 0; i < UnitSize; i++) {
        City_Position[i].x = m_city.at(i).m_x;
        City_Position[i].y = m_city.at(i).m_y;
    }

    std::vector<int> vec_pop;

    for (int i = 0; i < UnitSize; i++) {
        vec_pop.push_back(i);
    }

    //标记起始点和终止点Begin，Blank，id为m_begin_id，m_blank_id
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
    //std::cout << "算法初始化完成" << std::endl;
}

std::vector<Node_TSP>& GA_TSP::GA() {
    unsigned seed;
    seed = time(0);
    srand(seed);

    calculate_City_Distance();
    //初始化种群
    initialize_population();
    //计算适应值
    calculate_fitness();
    //迭代
    for (m_count = 0; m_count <= GenerationsMaxN; m_count++) {
        //选择
        select();
        //交叉
        cross();
        //变异
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

//计算城市距离词典City_Distance[i][j]
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

//数组复制
inline void copy(int a[], int b[]) {
    int i = 0;
    for (i = 0; i < UnitSize; i++) {
        a[i] = b[i];
    }
}

//种群初始化
void GA_TSP::initialize_population() {
    int i, j, r;
    gbest = MaxValue;
    m_best_distance = MaxValue;
    m_best_fitness = 0;//适应值越大越好
}

//计算适应值
void GA_TSP::calculate_fitness() {
    int i, j;
    int start, end;
    int best_index = 0;
    for (i = 0; i < PopSize; i++) {//求每个个体的总路径，适应值
        m_distance[i] = 0;
        for (j = 1; j < UnitSize; j++) {
            start = m_population[i][j - 1]; end = m_population[i][j];
            m_distance[i] += City_Distance[start][end];//city.Distance[i]每个个体的总路径
        }
        m_fitness[i] = ValuetoFitness / m_distance[i];
        if (m_fitness[i] > m_fitness[best_index])//选出最大的适应值，即选出所有个体中的最短路径
            best_index = i;
    }

    copy(m_best_solution, m_population[best_index]);//将最优个体拷贝给city.BestRooting
    m_best_fitness = m_fitness[best_index];
    m_best_distance = m_distance[best_index];
    m_best_index = best_index;
    if (m_best_distance < gbest)
        gbest = m_best_distance;

}

//选择算子
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
    //void *memcpy(void *dest, const void *src, size_t n)从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中
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

//交叉算子
void GA_TSP::cross() {
    int i, j, t, l;
    int a, b, ca, cb;
    int Temp1[UnitSize], Temp2[UnitSize];
    for (i = 0; i < PopSize; i++) {
        double s = ((double)(rand() % RAND_MAX)) / RAND_MAX;
        if (s < p_cross) {
            cb = rand() % PopSize;
            ca = cb;
            if (ca == m_best_index || cb == m_best_index)//如果遇到最优则直接进行下次循环
                continue;
            do {
                l = rand() % (UnitSize - 2) + 1;  //1-11
                a = rand() % (UnitSize - 2) + 1;  //1-20
            } while ((a + l) >= UnitSize);

            memset(Hash, 0, sizeof(Hash));//void *memset(void *s, int ch, size_t n);将s中当前位置后面的n个字节 用 ch 替换并返回 s 。
            Temp1[0] = m_begin1_id;
            Temp1[UnitSize - 1] = m_begin2_id;
            Hash[Temp1[0]] = 1;
            Hash[Temp1[UnitSize - 1]] = 1;
            for (j = 1; j <= l; j++) {//打乱顺序即随机，选出来的通过Hash标记为1
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

//变异算子
void GA_TSP::mutate() {
    int i, k, m;
    int Temp[UnitSize];
    for (k = 0; k < PopSize; k++) {
        double s = ((double)(rand() % RAND_MAX)) / RAND_MAX;//随机产生概率0~1间
        i = rand() % PopSize;//随机产生0~POPSIZE之间的数
        if (s < p_mutate && i != m_best_index) {//i!=city.BestNum，即保证最优的个体不变异
            int a, b, t;
            a = (rand() % (UnitSize - 2)) + 1;
            b = (rand() % (UnitSize - 2)) + 1;
            copy(Temp, m_population[i]);
            if (a > b) {//保证让b>=a
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

//获得适应值
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

//输出
void GA_TSP::output() {
    int i, j;
    std::cout << "最佳路径为:" << std::endl;
    for (i = 0; i < UnitSize; i++) {
        std::cout << m_best_solution[i] << ' ';
    }
    std::cout << std::endl;
    std::cout << "最佳路径值为:" << m_best_distance << std::endl;
}

//输出绘图信息
void GA_TSP::output_draw() {
    std::ofstream fout;
    //第0代数据有误，剔除不进行输出
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
    //std::cout << "最优解的数据已经输出至best.txt" << std::endl;
}
