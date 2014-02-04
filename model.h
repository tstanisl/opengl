#ifndef MODEL_H
#define MODEL_H

struct vertex {
	float position[3];
	float texture[2];
	float normal[3];
};

struct model {
	int n_element;
	int *element;
	int n_vertex;
	struct vertex vertex[];
};

struct model *model_load(char *path);
void model_put(struct model *m);

#endif /* MODEL_H */
