#include "CollisionDetectionNP.hpp"

CollisionDetectionNP::CollisionDetectionNP() {

}

CollisionDetectionNP::~CollisionDetectionNP() {
}



// input is a collection of CARS' collection of face vertices
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
						colFaces[j][k / 4] = true; // check this
					}
				}


			}
		}
	}

	return colFaces;
}
