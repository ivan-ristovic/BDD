#ifndef BDD_H
#define BDD_H

#include <memory>

class BDDNode;
using BDD = std::shared_ptr<BDDNode>;


class BDDNode
{
public:
    BDDNode();

private:
    bool value;
    BDD m_low;
    BDD m_high;
};

#endif // BDD_H
