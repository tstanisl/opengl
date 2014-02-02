#ifndef MODEL_H
#define MODEL_H

struct vertex {
	float position[3];
	float texcoord[2];
	float normal[3];
};

struct model {
	struct vertex *vertex;
	int *element;
	int n_vertex;
	int n_elements;
};

struct model *model_load(char *path);
void model_put(struct model *m);

#endif /* MODEL_H */
