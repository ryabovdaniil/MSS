#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::queue;

const int FREE = -1;
const int WAIT = -2;

//Структура судна
struct Ship {
public:
    int id;
    int priority;
    int arrivalTime;

    vector<int> port = {0,0};

    Ship(int _id, bool _priority, int _arrivalTime) : id(_id), priority(_priority), arrivalTime(_arrivalTime) {}

};
\


class PortSimulation {
public:
    vector<int> New = { FREE, FREE, FREE, FREE,FREE };
    vector<int> krNew = { FREE, FREE, FREE };
    vector<int> Old = { FREE, FREE };
    vector<int> krOld = { FREE, FREE };

    int useTimeKrOld = 0;
    int useTimeKrNew = 0;

    int countPortOld = 0;
    int countPortNew = 0;

    int shipsWithPriority = 0;
    int shipsWithoutPriority = 0;

    int pr1StartSum = 0;
    int pr0StartSum = 0;
    int pr1EndSum = 0;
    int pr0EndSum = 0;

    queue <Ship> ships;

    vector <Ship> buffer;

    vector <Ship> waitInDockShips;

    int simulationTime = 0;
    int maxRaidShips = 0;

    vector <int> sumServeTime = { 0,0 };
    vector <queue<int>> finishServeTimeNew;
    vector <queue<int>> finishServeTimeOld;
    vector <queue<int>> serveTime;

    
    PortSimulation() : finishServeTimeNew(3), finishServeTimeOld(2), serveTime(2) {}

    //Функция сортировки очереди по приоритетам
    void mysort(queue<Ship>& ships) {
        buffer.clear();
        while (ships.size()!=0)
        {
            buffer.push_back(ships.front());
            ships.pop();
        }
        sort(buffer.begin(), buffer.end(), [](const Ship& a, const Ship& b)->bool {return a.priority > b.priority; });
        for (size_t i = 0; i < buffer.size() ; i++)
        {
            ships.push(buffer[i]);
        }
    }

    //Функция вычисления времени швартовки и обслуживания судов портами
    int advance(int mode = 5) {
        switch (mode)
        {
        case 0:
            return rand() % 11 + 15;
        case 1:
            return rand() % 21 + 30;
        default:
            return 1;
        }
    }

    

    //Проверка пустых портов
    bool docksFree() {
        for (size_t i = 0; i < New.size(); i++)
        {
            if(New[i] != FREE) {
                return false;
            }
        }
        for (size_t i = 0; i < Old.size(); i++)
        {
            if (Old[i] != FREE) {
                return false;
            }
        }
        return true;
    }

    //Функция занимающая порты и краны
    bool takePort(vector<int>& port,int& place, int id) {
        for (int i = 0; i < port.size(); i++) 
        {
            if (port[i]==FREE)
            {
                port[i] = id;
                place = i;

                return true;
            }
        }
        return false;
    }

    //Функция начала обслуживания судов
    void start(int port,int place) {
        int ts = advance(port);
        sumServeTime[port] += ts;
        serveTime[port].push(ts);
        if (port == 0)
        {
            finishServeTimeNew[place].push(simulationTime + ts);
        }
        else
        {
            finishServeTimeOld[place].push(simulationTime + ts);
        }
        
    }

    //Функция обслуживания судов
    void solve(bool allGen) {

        if (maxRaidShips<ships.size())
        {
            maxRaidShips = ships.size();
        }

        int id;
        for (size_t i = 0; i < finishServeTimeNew.size(); i++)
        {
            if (finishServeTimeNew[i].size() != 0 && simulationTime >= finishServeTimeNew[i].front())
            {

                finishServeTimeNew[i].pop();
                serveTime[0].pop();
                countPortNew++;
                id = krNew[i];
                krNew[i] = FREE;

                if (waitInDockShips.size() != 0 && takePort(krNew, waitInDockShips[0].port[1], waitInDockShips[0].id))
                {
                    start(0, waitInDockShips[0].port[1]);
                    waitInDockShips.erase(waitInDockShips.begin());
                }

                for (int& n : New) {
                    if (n == id) {
                        n = FREE;
                        break;
                    }
                }
            }
        }
        for (size_t i = 0; i < finishServeTimeOld.size(); i++)
        {
            if (finishServeTimeOld[i].size() != 0 && simulationTime >= finishServeTimeOld[i].front())
            {
                finishServeTimeOld[i].pop();
                serveTime[1].pop();
                countPortOld++;
                
                krOld[i] = FREE;
                Old[i] = FREE;                
            }
        }
    }

    //Функция запуска моделирования
    void simulate(int totalShips) {
        srand(time(nullptr));
        
        for (int i = 0; i < totalShips; i++) 
        {
            int priorityPossibility = rand() % 100 + 1;
            int priority = (priorityPossibility <= 40) ? 1 : 0;
            if (priority == 1)
            {
                shipsWithPriority++;
            }
            else
            {
                shipsWithoutPriority++;
            }
            int time = rand() % 7 + 2;
            simulationTime += time;
            Ship currentShip = Ship(i, priority, simulationTime);
            
            solve(false);

            if (ships.size() == 0)
            {   
                if (takePort(New,currentShip.port[0],currentShip.id))
                {
                    if (takePort(krNew, currentShip.port[1], currentShip.id))
                    {                    
                        start(0, currentShip.port[1]);
                    }
                    else
                    {
                    
                        currentShip.port[1] = WAIT;
                        waitInDockShips.push_back(currentShip);
                    }
                }
                else if (takePort(Old, currentShip.port[0], currentShip.id))
                {   
                    takePort(krOld, currentShip.port[1], currentShip.id);
                    start(1, currentShip.port[1]);
                }
                else
                {
                    ships.push(currentShip);
                    if (currentShip.priority == 1)
                    {
                        pr1StartSum += simulationTime;
                    }
                    else
                    {
                        pr0StartSum += simulationTime;
                    }

                }
            }
            else
            {
                ships.push(currentShip);
                if (currentShip.priority == 1)
                {
                    pr1StartSum += simulationTime;
                }
                else
                {
                    pr0StartSum += simulationTime;
                }
                mysort(ships);

                for (int i = 0; i < ships.size(); i++)
                {
                    if (takePort(New, ships.front().port[0], ships.front().id))
                    {
                        if (ships.front().priority == 1)
                        {
                            pr1EndSum += simulationTime;
                        }
                        else
                        {
                            pr0EndSum += simulationTime;
                        }
                        
                        if (takePort(krNew, ships.front().port[1], ships.front().id))
                        {
                            start(0, ships.front().port[1]);
                            ships.pop();
                        }
                        else
                        {
                            ships.front().port[1] = WAIT;
                            waitInDockShips.push_back(ships.front());
                            ships.pop();
                        }
                        i--;
                    }
                    else if (takePort(Old, ships.front().port[0], ships.front().id))
                    {                        
                        takePort(krOld, ships.front().port[1], ships.front().id);
                        start(1, ships.front().port[1]);
                        if (ships.front().priority == 1)
                        {
                            pr1EndSum += simulationTime;
                        }
                        else
                        {
                            pr0EndSum += simulationTime;
                        }

                        ships.pop();
                        i--;
                    }
                    else
                    {
                        break;
                    }
                    
                }
                                
            }
        }

        while (ships.size() != 0) {
            solve(true);

            for (int i = 0; i < ships.size(); i++)
            {
                if (takePort(New, ships.front().port[0], ships.front().id))
                {
                    if (ships.front().priority == 1)
                    {
                        pr1EndSum += simulationTime;
                    }
                    else
                    {
                        pr0EndSum += simulationTime;
                    }

                    if (takePort(krNew, ships.front().port[1], ships.front().id))
                    {
                        start(0, ships.front().port[1]);                  
                        ships.pop();
                    }
                    else
                    {
                        ships.front().port[1] = WAIT;
                        waitInDockShips.push_back(ships.front());                        
                        ships.pop();
                    }
                    i--;
                }
                else if (takePort(Old, ships.front().port[0], ships.front().id))
                {
                    takePort(krOld, ships.front().port[1], ships.front().id);
                    start(1, ships.front().port[1]);
                    if (ships.front().priority == 1)
                    {
                        pr1EndSum += simulationTime;
                    }
                    else
                    {
                        pr0EndSum += simulationTime;
                    }

                    ships.pop();
                    i--;
                }
                else
                {
                    break;
                }

            }
            simulationTime++;
        }

        while (docksFree()==false)
        {
            solve(true);
            simulationTime++;
        }

        cout << "Общее время моделирования: " << simulationTime << endl;
        cout << "-----------------------------------------------------------------" << endl;
        cout << "Судов с приоритетом: " << shipsWithPriority << "\t\t| " << "Судов без приоритета: " << shipsWithoutPriority << endl;
        cout << "-----------------------------------------------------------------" << endl;
        cout << "Обслужено новым причалом: " << countPortNew << "\t| " << "Обслужено старым причалом: " << countPortOld << endl;
        cout << "-----------------------------------------------------------------" << endl;


        cout << "Коэффициент загруженности нового причала: "
            << (float)(sumServeTime[0]/5 )/ simulationTime * 100.0 << "%" << endl;

        cout << "Коэффициент загруженности старого причала: "
            << (float)(sumServeTime[1]/2 ) / simulationTime * 100.0 << "%" << endl;
        cout << "-----------------------------------------------------------------" << endl;

        cout << "Максимальное число судов в рейде: " << maxRaidShips << endl;
        cout << "-----------------------------------------------------------------" << endl;


        cout << "Ожидание в рейде для судов с приоритетом: "
            << (pr1EndSum-pr1StartSum)/shipsWithPriority << endl;

        cout << "Ожидание в рейде для судов без приоритета: "
            << (pr0EndSum - pr0StartSum) / shipsWithoutPriority << endl;
    }
};

int main() {
    setlocale(LC_ALL, "ru");
    PortSimulation simulation;
    simulation.simulate(150);

    return 0;
}
