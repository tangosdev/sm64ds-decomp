//cpp
// @symbol _ZN15BookShotSpawnerD1Ev

struct Actor {
    virtual ~Actor();
};


struct BookShotSpawner : Actor {
    virtual ~BookShotSpawner();
};

BookShotSpawner::~BookShotSpawner()
{
}
