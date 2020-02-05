namespace keyvaluestore {

class Iterator {
    
    public:

    Iterator();
    virtual ~Iterator();

    virtual bool hasNext() = 0;
    virtual void next() = 0;
    virtual string key() = 0;
    virtual string value() = 0;



}