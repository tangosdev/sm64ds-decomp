//cpp
// @symbol _ZN12EnemySpawnerD1Ev

struct Actor {
    virtual ~Actor();
};


struct EnemySpawner : Actor {
    virtual ~EnemySpawner();
};

EnemySpawner::~EnemySpawner()
{
}
