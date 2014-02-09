#ifndef MODEL_H
#define MODEL_H

struct vertex {
	float position[3];
	float texture[2];
	float normal[3];
};

struct model {
	int n_element;
	unsigned int *element;
	int n_vertex;
	struct vertex vertex[];
};

struct model *model_load(char *path);
void model_put(struct model *m);
void model_compute_normals(struct model *m);

#endif /* MODEL_H */
