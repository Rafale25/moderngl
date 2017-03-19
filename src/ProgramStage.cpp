#include "ProgramStage.hpp"

#include "Error.hpp"
#include "Subroutine.hpp"
#include "SubroutineUniform.hpp"

PyObject * MGLProgramStage_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLProgramStage * self = (MGLProgramStage *)type->tp_alloc(type, 0);

	#ifdef MGL_VERBOSE
	printf("MGLProgramStage_tp_new %p\n", self);
	#endif

	if (self) {
	}

	return (PyObject *)self;
}

void MGLProgramStage_tp_dealloc(MGLProgramStage * self) {

	#ifdef MGL_VERBOSE
	printf("MGLProgramStage_tp_dealloc %p\n", self);
	#endif

	Py_TYPE(self)->tp_free((PyObject *)self);
}

int MGLProgramStage_tp_init(MGLProgramStage * self, PyObject * args, PyObject * kwargs) {
	MGLError * error = MGLError_New(TRACE, "Cannot create ModernGL.ProgramStage manually");
	PyErr_SetObject((PyObject *)&MGLError_Type, (PyObject *)error);
	return -1;
}

PyObject * MGLProgramStage_tp_str(MGLProgramStage * self) {
	return PyUnicode_FromFormat("<ModernGL.ProgramStage>");
}

PyMethodDef MGLProgramStage_tp_methods[] = {
	{0},
};

PyObject * MGLProgramStage_get_subroutines(MGLProgramStage * self, void * closure) {
	Py_INCREF(self->subroutines);
	return self->subroutines;
}

PyObject * MGLProgramStage_get_subroutine_uniforms(MGLProgramStage * self, void * closure) {
	Py_INCREF(self->subroutine_uniforms);
	return self->subroutine_uniforms;
}

PyGetSetDef MGLProgramStage_tp_getseters[] = {
	{(char *)"subroutines", (getter)MGLProgramStage_get_subroutines, 0, 0, 0},
	{(char *)"subroutine_uniforms", (getter)MGLProgramStage_get_subroutine_uniforms, 0, 0, 0},
	{0},
};

const char * MGLProgramStage_tp_doc = R"(
	ProgramStage
)";

PyTypeObject MGLProgramStage_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"ModernGL.ProgramStage",                                // tp_name
	sizeof(MGLProgramStage),                                // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLProgramStage_tp_dealloc,                 // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	(reprfunc)MGLProgramStage_tp_str,                       // tp_repr
	0,                                                      // tp_as_number
	0,                                                      // tp_as_sequence
	0,                                                      // tp_as_mapping
	0,                                                      // tp_hash
	0,                                                      // tp_call
	(reprfunc)MGLProgramStage_tp_str,                       // tp_str
	0,                                                      // tp_getattro
	0,                                                      // tp_setattro
	0,                                                      // tp_as_buffer
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,               // tp_flags
	MGLProgramStage_tp_doc,                                 // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLProgramStage_tp_methods,                             // tp_methods
	0,                                                      // tp_members
	MGLProgramStage_tp_getseters,                           // tp_getset
	&MGLProgramMember_Type,                                 // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLProgramStage_tp_init,                      // tp_init
	0,                                                      // tp_alloc
	MGLProgramStage_tp_new,                                 // tp_new
};

MGLProgramStage * MGLProgramStage_New() {
	MGLProgramStage * self = (MGLProgramStage *)MGLProgramStage_tp_new(&MGLProgramStage_Type, 0, 0);
	return self;
}

void MGLProgramStage_Complete(MGLProgramStage * program_stage) {
	const GLMethods & gl = program_stage->program->context->gl;

	PyObject * subroutines = PyDict_New();
	PyObject * subroutine_uniforms = PyDict_New();

	int shader_type = program_stage->shader->shader_type;

	int num_subroutines = 0;
	gl.GetProgramStageiv(program_stage->program->program_obj, shader_type, GL_ACTIVE_SUBROUTINES, &num_subroutines);

	int num_subroutine_uniforms = 0;
	gl.GetProgramStageiv(program_stage->program->program_obj, shader_type, GL_ACTIVE_SUBROUTINE_UNIFORMS, &num_subroutine_uniforms);

	int num_subroutine_uniform_locations = 0;
	gl.GetProgramStageiv(program_stage->program->program_obj, shader_type, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &num_subroutine_uniform_locations);

	// TODO: create array [subroutine_uniform_locations]

	for (int i = 0; i < num_subroutines; ++i) {
		MGLSubroutine * subroutine = MGLSubroutine_New();

		int name_len = 0;
		char name[256];

		gl.GetActiveSubroutineName(program_stage->program->program_obj, shader_type, i, 256, &name_len, name);

		subroutine->index = gl.GetSubroutineIndex(program_stage->program->program_obj, shader_type, name);

		subroutine->number = i;
		subroutine->program_stage = program_stage;
		subroutine->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLSubroutine_Complete(subroutine);

		PyDict_SetItem(subroutines, subroutine->name, (PyObject *)subroutine);
		Py_DECREF(subroutine);
	}

	for (int i = 0; i < num_subroutine_uniforms; ++i) {
		MGLSubroutineUniform * subroutine_uniform = MGLSubroutineUniform_New();

		int name_len = 0;
		char name[256];

		gl.GetActiveSubroutineUniformName(program_stage->program->program_obj, shader_type, i, 256, &name_len, name);

		subroutine_uniform->index = gl.GetSubroutineUniformLocation(program_stage->program->program_obj, shader_type, name);

		subroutine_uniform->number = i;
		subroutine_uniform->program_stage = program_stage;
		subroutine_uniform->name = PyUnicode_FromStringAndSize(name, name_len);

		MGLSubroutineUniform_Complete(subroutine_uniform);

		PyDict_SetItem(subroutine_uniforms, subroutine_uniform->name, (PyObject *)subroutine_uniform);
		Py_DECREF(subroutine_uniform);
	}

	program_stage->subroutines = PyDictProxy_New(subroutines);
	program_stage->subroutine_uniforms = PyDictProxy_New(subroutine_uniforms);
}
