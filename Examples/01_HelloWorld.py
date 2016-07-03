# python -m pip install ModernGL
# python -m pip install GLWindow

import ModernGL as GL
import GLWindow as WND
import struct

WND.InitializeWindow()
WND.BuildFullscreen()
GL.InitializeModernGL()

vert = GL.NewVertexShader('''
	#version 400

	in vec2 vert;

	void main() {
		gl_Position = vec4(vert, 0.0, 1.0);
	}
''')

frag = GL.NewFragmentShader('''
	#version 400
	
	out vec4 color;

	void main() {
		color = vec4(0.30, 0.50, 1.00, 1.0);
	}
''')

prog = GL.NewProgram([vert, frag])

vbo = GL.NewVertexBuffer(struct.pack('6f', 0.0, 0.8, -0.6, -0.8, 0.6, -0.8))
vao = GL.NewVertexArray('2f', [(vbo, GL.GetAttributeLocation(prog, 'vert'))])

while WND.Update():
	GL.Clear(240, 240, 240)

	GL.UseProgram(prog)
	GL.RenderTriangles(vao, 3)