#ifndef SKIP_LIST_H
#define SKIP_LIST_H

// Skip Node
typedef struct quadNode {
  int data;
  struct quadNode *prev, *next, *above, *below;
} QuadNode;

// A struct for keeping track of a skip list's head and potentially tail and length
typedef struct {
  QuadNode   *head;
  int maxHeight;
} SkipList;

void skipListInsert(SkipList *list,  int key);
void skipListDelete(SkipList *list,  int key);
QuadNode* skipListFind(SkipList *list, int key);
void printSkipList(SkipList *list);

#endif




