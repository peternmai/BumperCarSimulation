#include "CollisionDetectionNP.hpp"

CollisionDetectionNP::CollisionDetectionNP() {

}

CollisionDetectionNP::~CollisionDetectionNP() {
}



/* This is the collision detection algorithm checking if whether or not a point is within another
** bounding box. This is done using the fact that the determinant of the point - the point in the plane
** is negative if it behind the plane. So we check all 4 planes of the bounding box. If it matches all
** requirements, then there is collision
**
** Input: a vector of vectors of points, a collection of each cars' points
** Output: a vector of vector of bools, a collection of each cars' bools saying whether or not
**		   a face has been collided with
*/
std::vector<std::vector<bool>> CollisionDetectionNP::intersectionTest(std::vector<std::vector<glm::vec3>> minmaxpts) {
	std::vector<std::vector<bool>> colFaces;

	for (unsigned int i = 0; i < minmaxpts.size(); i++) {
		std::vector<bool> temp(4, false);
		colFaces.push_back(temp);
	}

	for (unsigned int i = 0; i < minmaxpts.size(); i++) {
		std::vector<glm::vec3> temp1 = minmaxpts[i];
		
		for (unsigned int j = 0; j < minmaxpts.size(); j++) {
			std::vector<glm::vec3> temp2 = minmaxpts[j];

			if (i == j) continue;

			for (unsigned int k = 0; k < temp1.size(); k = k + 4) {
				glm::vec3 ppt0 = temp1[0 + k];
				glm::vec3 ppt1 = temp1[1 + k];
				glm::vec3 ppt2 = temp1[2 + k];

				glm::vec3 v1 = ppt0 - ppt1;
				glm::vec3 v2 = ppt0 - ppt2;

				for (unsigned int l = 0; l < temp2.size(); l++) {

					glm::vec3 pptx = temp2[l];
					glm::vec3 vx = ppt0 - pptx;

					glm::mat3 detM = glm::mat3(v1, v2, vx);

					float det = glm::determinant(detM);

					if (det >= 0) {
						colFaces[j][k / 4] = true;
					}
				}


			}
		}
	}

	return colFaces;
}
