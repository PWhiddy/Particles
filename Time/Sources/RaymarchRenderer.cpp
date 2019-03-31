#include "RaymarchRenderer.h"

namespace rme
{

	Controls *control = new Controls();

	RaymarchRenderer::RaymarchRenderer(int w, int h)
	{
		width = w;
		height = h;

		// Init GLFW
		glfwInit();
		// Set all the required options for GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		// Create a GLFWwindow object that we can use for GLFW's functions
		window = glfwCreateWindow(width, height, "Time", nullptr, nullptr);

	    // Check for Valid Context
	    if (window == nullptr) {
	        throw std::runtime_error("Failed to Create OpenGL Context");
	    }

		glfwMakeContextCurrent(window);

		// Disable mouse 
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Set the required callback functions
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, cursor_pos_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);

		//// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
		//glewExperimental = GL_TRUE;
		// Initialize GLEW to setup the OpenGL Function pointers
		//glewInit();
		gladLoadGL();

		// get version info
		const GLubyte* hardware = glGetString(GL_RENDERER); // get renderer string
		const GLubyte* version = glGetString(GL_VERSION); // version as a string
		printf("Hardware: %s\n", hardware);
		printf("OpenGL version supported %s\n", version);

		// Define the viewport dimensions
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Load shaders
		std::string vert = loadSource((char*)("../../Time/Shaders/pass.vert"));
		std::string frag = loadSource((char*)("../../Time/Shaders/march.frag"));
		//std::printf("source: %s\n", vert.c_str());
		const GLchar* vertexShaderSource = (const GLchar *)vert.c_str();
		const GLchar* fragmentShaderSource = (const GLchar *)frag.c_str();

		// Build and compile our shader program
		// Vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		// Check for compile time errors
		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// Check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		Particle party[particleCount];
		
		for (int i=0; i<particleCount; i++) {
			float fi = 2.0*3.14159*static_cast<float>(i)/static_cast<float>(particleCount);
			party[i].position.x = 0.65*cosf(12.0*fi);
			party[i].position.y = 0.65*sinf(13.0*fi);
			party[i].position.z = 0.05;
			party[i].velocity.x = 0.01;
			party[i].velocity.y = 0.0;
			party[i].velocity.z = 0.0;
		}
		parts = party;

		// Set up vertex data (and buffer(s)) and attribute pointers
		vertices = new float[arrSize];
		convertPartsToTriangles();
		/*
		for (int i=0; i<particleCount; i++) {
			int idx = i*3;
			float fi = 2.0*3.14159*static_cast<float>(i)/static_cast<float>(particleCount);
			vertices[idx+0] = cosf(fi);
			vertices[idx+1] = sinf(fi);
			vertices[idx+2] = 0.0f;
		}
		*/
		/*
		{
			/*
			1.0f, 1.0f, 0.0f,  // Top Right
			1.0f, -1.0f, 0.0f,  // Bottom Right
			-1.0f, -1.0f, 0.0f,  // Bottom Left
			-1.0f, 1.0f, 0.0f   // Top Left 
			* /
			0.5f, 0.9f, 0.0f,  // Top Right
			0.1f, 0.4f, 0.0f,  // Bottom Right
			0.8f, 0.0f, 0.0f  // Bottom Left
		//	0.3f, 0.7f, 0.0f   // Top Left 
		};
		*/
		/*
		GLuint indices[] = {  // Note that we start from 0!
			0, 1, 3,  // First Triangle
		//	1, 2, 3   // Second Triangle
		};
		*/
		//GLuint VBO, VAO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		//glGenBuffers(1, &EBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*arrSize, &vertices[0], GL_DYNAMIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to 
		// glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		//glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any 
		// buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

		glUseProgram(shaderProgram);

		timeLocation = glGetUniformLocation(shaderProgram, "time");
		resolutionLocation = glGetUniformLocation(shaderProgram, "resolution");
		//objCountLocation = glGetUniformLocation(shaderProgram, "objectCount");
		rotationLocation = glGetUniformLocation(shaderProgram, "cameraRotation");
		camPosLocation = glGetUniformLocation(shaderProgram, "cameraPos");
		//warpCountLoc = glGetUniformLocation(shaderProgram, "warpCount");

		//glUniform1i(warpCountLoc, 0);

		glUniform2f(resolutionLocation, (GLfloat)width, (GLfloat)height);

		//glUniform1i(objCountLocation, 0);

		glUniform2f(rotationLocation, 0.0, 0.0);

		//getObject3DLocation(&warpALoc, "warpA");
		//getObject3DLocation(&warpBLoc, "warpB");

		//objectLocations = new shaderObject3D[MAX_OBJECTS];


		glfwSetTime(0.0);

	}

	void RaymarchRenderer::convertPartsToTriangles()
	{
		for(int i=0; i<particleCount; i++) {
			for (int t=0; t<trianglesPerPart; t++) {
				int partIdx = i*dimsPerVert*vertsPerTriangle*trianglesPerPart + t*dimsPerVert*vertsPerTriangle;
				Particle p = parts[i];

				vertices[partIdx+0] = p.position.x;
				vertices[partIdx+1] = p.position.y+partSize;
				vertices[partIdx+2] = p.position.z;

				vertices[partIdx+3] = p.position.x-partSize;
				vertices[partIdx+4] = p.position.y;
				vertices[partIdx+5] = p.position.z;

				vertices[partIdx+6] = p.position.x+partSize;
				vertices[partIdx+7] = p.position.y;
				vertices[partIdx+8] = p.position.z;
			}
		}
	}


	void RaymarchRenderer::render(Scene* scene, Camera* camera)
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) 
		// and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Update uniforms with Scene

		updateUniforms(scene, camera);

		float t = 0.0f;//float(glfwGetTime());
		/*
		vertices[0] = sinf(t);
		vertices[1] = cosf(t);
		vertices[2] = sinf(2.0f*t);
		vertices[3] = sinf(3.0*t);
		vertices[4] = cosf(3.0*t);
		vertices[5] = sinf(2.5f*t);
		vertices[6] = sinf(1.2*t);
		vertices[7] = cosf(1.4*t);
		vertices[8] = sinf(0.5f*t);
		*/

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*arrSize, &vertices[0], GL_DYNAMIC_DRAW);
		//glUseProgram(shaderProgram);
		//glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, particleCount*3);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_POINTS, 3, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);

	}

	void RaymarchRenderer::updateUniforms(Scene* scene, Camera* camera)
	{
		glUniform1f(timeLocation, float(glfwGetTime()));
		glUniform2f(rotationLocation, control->xRotation, control->yRotation);
		glUniform3f(camPosLocation, camera->position.x, camera->position.y, camera->position.z);
		//int size = scene->children.size();
		//glUniform1i(objCountLocation, size);

	}

	Controls* RaymarchRenderer::getControl()
	{
		return control;
	}

	std::string RaymarchRenderer::loadSource(char* filename)
	{
		std::ifstream infile{ filename };
		return{ std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() };
	}

	RaymarchRenderer::~RaymarchRenderer()
	{
		// Properly de-allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		delete vertices;
		// Terminate GLFW, clearing any resources allocated by GLFW.
		glfwTerminate();
	}

	/// Connect Controls ///

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		control->interpretKey(scancode, action);
		std::printf("keypress: %i  scancode: %i  action: %i  mode: %i\n", key, scancode, action, mode);
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		std::cout << "button: " << button << " action : " << action << "\n";
		control->interpretMouseButton(button, action);
	}

	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
	{
		//std::cout << "xpos: " << xpos << " ypos: " << ypos << "\n";
		control->interpretMouseMove(xpos, ypos);
	}

}
