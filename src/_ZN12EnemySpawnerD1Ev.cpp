//cpp
// @symbol _ZN12EnemySpawnerD1Ev

struct dActor_c {
    virtual ~dActor_c();
};


struct EnemySpawner : dActor_c {
    virtual ~EnemySpawner();
};

EnemySpawner::~EnemySpawner()
{
}
