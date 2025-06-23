#ifndef CMN_OBJECT_H
#define CMN_OBJECT_H

class CmnObject : public Scriptable {
public:
	CmnObject(){}

	glm::vec2 pos = glm::ivec2(0, 0);

};

#endif