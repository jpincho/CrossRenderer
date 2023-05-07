#include "OpenGLShader.hpp"
#include "OpenGLInternals.hpp"
#include "../Internal/Logger.hpp"
#include "../Internal/ConsoleTextColor.hpp"
#include "../Internal/StringUtils.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
bool ParseErrorMessages ( const std::string &Code, const std::string &Infolog, std::vector <std::string> &Output );
bool DetectUniformsAndAttributes ( GLuint OpenGLID, std::vector <UniformInfo> &Uniforms, std::vector <AttributeInfo> &Attributes );

ShaderObjectHandle CreateShaderObject ( const ShaderObjectType Type )
	{
	GLenum GLShaderTypes[] = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };

	ShaderObjectInfo NewShaderObject;
	NewShaderObject.OpenGLID = glCreateShader ( GLShaderTypes[ ( int ) Type] );
	NewShaderObject.Type = Type;
	ShaderObjectHandle NewHandle ( ShaderObjects.GetFreeIndex () );
	ShaderObjects[NewHandle.GetKey ()] = NewShaderObject;
	return NewHandle;
	}

void DeleteShaderObject ( const ShaderObjectHandle Handle )
	{
	ShaderObjectInfo *ShaderObjectInformation = &ShaderObjects[Handle.GetKey()];

	glDeleteShader ( ShaderObjectInformation->OpenGLID );
	ShaderObjects.ReleaseIndex ( Handle.GetKey () );
	}

bool BuildShaderObject ( const ShaderObjectHandle Handle, const std::string &Code )
	{
	ShaderObjectInfo *ShaderObjectInformation = &ShaderObjects[Handle.GetKey ()];
	LOG_DEBUG ( "Compiling shader object" );

	// Loads the shader source code
	ShaderObjectInformation->Code = Code;
	const char *Sources[1];
	Sources[0] = ShaderObjectInformation->Code.c_str ();

	int CompileStatus = 0;
	glShaderSource ( ShaderObjectInformation->OpenGLID, ( GLsizei ) 1, Sources, nullptr );
	glCompileShader ( ShaderObjectInformation->OpenGLID );
	glGetShaderiv ( ShaderObjectInformation->OpenGLID, GL_COMPILE_STATUS, &CompileStatus );

	if ( ( CheckError () == false ) || ( CompileStatus != GL_TRUE ) )
		{
		std::string Infolog;
		int InfoLogLength;
		glGetShaderiv ( ShaderObjectInformation->OpenGLID, GL_INFO_LOG_LENGTH, &InfoLogLength );
		if ( InfoLogLength )
			{
			Infolog.resize ( InfoLogLength );
			glGetShaderInfoLog ( ShaderObjectInformation->OpenGLID, InfoLogLength, nullptr, ( GLchar * ) Infolog.c_str () );
			}

		std::vector <std::string> ErrorStrings;
		if ( ( Infolog.length () > 0 ) && ( ParseErrorMessages ( Code, Infolog, ErrorStrings ) ) )
			{
			LOG_ERROR ( "OpenGL error during shader object compilation" );
			for ( const auto &ErrorStringIterator : ErrorStrings )
				LOG_RAW ( "%s", ErrorStringIterator.c_str () );
			return false;
			}
		else
			{
			LOG_ERROR ( "OpenGL error during shader object compilation. Unable to parse log. '%s'", Infolog.c_str () );
			LOG_ERROR ( "%s", Code.c_str () );
			}
		}

	LOG_DEBUG ( "Shader object successfully compiled" );
	return true;
	}

bool ParseErrorMessages ( const std::string &Code, const std::string &Infolog, std::vector <std::string> &Output )
	{
	std::string TextColorReset, TextColorError;
	ConsoleTextColor::TextColor ( TextColorError, ConsoleTextColor::Color::Red );
	ConsoleTextColor::TextColorReset ( TextColorReset );

	std::vector <std::string> ErrorStrings;
	std::vector <std::string> SourceLines;
	StringUtils::Split ( Code, '\n', SourceLines );
	StringUtils::Split ( Infolog, '\n', ErrorStrings );
	size_t LastPrintedLine = 0;
	if ( ErrorStrings.size () == 0 )
		return false;
	for ( const auto &ErrorStringIterator : ErrorStrings )
		{
		unsigned long Column, Line;
#if defined (CROSS_RENDERER_TARGET_PLATFORM_WINDOWS)
		if ( sscanf_s ( ErrorStringIterator.c_str (), "ERROR: %lu:%lu", &Column, &Line ) == 2 )
#elif defined (CROSS_RENDERER_TARGET_PLATFORM_LINUX)
		if ( sscanf ( ErrorStringIterator.c_str (), "ERROR: %lu:%lu", &Column, &Line ) == 2 )
#endif
			{
			for ( ; LastPrintedLine < Line; ++LastPrintedLine )
				Output.push_back ( SourceLines[LastPrintedLine] );
			Output.push_back ( TextColorError + ErrorStringIterator + TextColorReset );
			}
		else
			Output.push_back ( TextColorError + ErrorStringIterator + TextColorReset );
		}
	for ( ; LastPrintedLine < SourceLines.size (); ++LastPrintedLine )
		Output.push_back ( SourceLines[LastPrintedLine] );
	return true;
	}

ShaderHandle CreateShader ( void )
    {
    ShaderInfo NewShader;
    NewShader.OpenGLID = glCreateProgram();
    if ( CheckError() == false )
        return ShaderHandle::Invalid;
	ShaderHandle NewHandle ( Shaders.GetFreeIndex () );
	Shaders[NewHandle.GetKey ()] = NewShader;
	return NewHandle;
	}

ShaderHandle CreateShader ( const ShaderCode &NewCode )
        {
	ShaderObjectHandle VertexShader, GeometryShader, FragmentShader;
	ShaderHandle NewHandle = CreateShader ();

	std::vector <ShaderObjectHandle> ShaderObjectsToLink;

	if ( !NewCode.VertexShader.empty () )
        {
		VertexShader = CreateShaderObject ( ShaderObjectType::Vertex );
		if ( BuildShaderObject ( VertexShader, NewCode.VertexShader ) == false )
			goto OnError;
		ShaderObjectsToLink.push_back ( VertexShader );
		}
	if ( !NewCode.GeometryShader.empty () )
		{
		GeometryShader = CreateShaderObject ( ShaderObjectType::Geometry );
		if ( BuildShaderObject ( GeometryShader, NewCode.GeometryShader ) == false )
			goto OnError;
		ShaderObjectsToLink.push_back ( GeometryShader );
		}
	if ( !NewCode.FragmentShader.empty () )
		{
		FragmentShader = CreateShaderObject ( ShaderObjectType::Fragment );
		if ( BuildShaderObject ( FragmentShader, NewCode.FragmentShader ) == false )
			goto OnError;
		ShaderObjectsToLink.push_back ( FragmentShader );
        }
	if ( LinkShader ( NewHandle, ShaderObjectsToLink ) == false )
		goto OnError;

    return NewHandle;
OnError:
	if ( VertexShader )
		DeleteShaderObject ( VertexShader );
	if ( FragmentShader )
		DeleteShaderObject ( FragmentShader );
	if ( GeometryShader )
		DeleteShaderObject ( GeometryShader );
	if ( NewHandle )
		DeleteShader ( NewHandle );
	return ShaderHandle::Invalid;
    }

bool DeleteShader ( const ShaderHandle Handle )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];
	for ( const auto &Iterator : ShaderInformation->AttachedShaderObjects )
		glDetachShader ( ShaderInformation->OpenGLID, ShaderObjects[Iterator.GetKey ()].OpenGLID );

    glDeleteProgram ( ShaderInformation->OpenGLID );
    Shaders.ReleaseIndex ( Handle.GetKey () );
    return true;
    }

bool LinkShader ( const ShaderHandle Handle, const std::vector <ShaderObjectHandle> &ObjectHandles )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];
	std::vector <GLuint> GLIDs;
	for ( const auto &Iterator : ShaderInformation->AttachedShaderObjects )
		glDetachShader ( ShaderInformation->OpenGLID, ShaderObjects[Iterator.GetKey ()].OpenGLID );
	for ( const auto &Iterator : ObjectHandles )
		glAttachShader ( ShaderInformation->OpenGLID, ShaderObjects[Iterator.GetKey ()].OpenGLID );
	ShaderInformation->AttachedShaderObjects = ObjectHandles;

	// Link it, and check it
	glLinkProgram ( ShaderInformation->OpenGLID );

	GLint LinkStatus;
	glGetProgramiv ( ShaderInformation->OpenGLID, GL_LINK_STATUS, &LinkStatus );
	if ( LinkStatus != GL_TRUE )
		{
		std::string Infolog;
		int InfoLogLength;
		glGetProgramiv ( ShaderInformation->OpenGLID, GL_INFO_LOG_LENGTH, &InfoLogLength );
		if ( InfoLogLength )
			{
			Infolog.resize ( InfoLogLength );
			glGetProgramInfoLog ( ShaderInformation->OpenGLID, InfoLogLength, nullptr, ( GLchar * ) Infolog.c_str () );
			}

		LOG_ERROR ( "OpenGL error during shader program linking. '%s'", Infolog.c_str () );
		for ( const auto &Iterator : ObjectHandles )
			{
			GLint length;
			glGetShaderiv ( ShaderObjects[Iterator.GetKey ()].OpenGLID, GL_SHADER_SOURCE_LENGTH, &length );
			std::string Source;
			Source.resize ( length );
			glGetShaderSource ( ShaderObjects[Iterator.GetKey ()].OpenGLID, length, &length, ( char * ) Source.c_str () );
			LOG_ERROR ( "%s", Source.c_str () );
			}

        return false;
		}
	LOG_DEBUG ( "Linked shader program %u", ShaderInformation->OpenGLID );
    if ( DetectUniformsAndAttributes ( ShaderInformation->OpenGLID, ShaderInformation->Uniforms, ShaderInformation->Attributes ) == false )
        {
        LOG_ERROR ( "Error detecting uniforms/attributes" );
        return false;
        }

    return true;
    }

void GetShaderUniformList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderUniformType>> &UniformList )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

    for ( const auto &iterator : ShaderInformation->Uniforms )
        UniformList.push_back ( std::pair <std::string, ShaderUniformType> ( iterator.Name, iterator.Type ) );
    }

ShaderUniformHandle GetShaderUniformHandle ( const ShaderHandle Handle, const std::string Name )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

    for ( unsigned cont = 0; cont < ShaderInformation->Uniforms.size(); ++cont )
        {
        if ( ShaderInformation->Uniforms[cont].Name == Name )
            return ShaderUniformHandle ( cont );
        }
	//LOG_ERROR ( "Invalid uniform '%s' for shader", Name.c_str() );
    return ShaderUniformHandle::Invalid;
    }

void GetShaderAttributeList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

    for ( const auto &iterator : ShaderInformation->Attributes )
        AttributeList.push_back ( std::pair <std::string, ShaderAttributeType> ( iterator.Name, iterator.Type ) );
    }

ShaderAttributeHandle GetShaderAttributeHandle ( const ShaderHandle Handle, const std::string Name )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

    for ( unsigned cont = 0; cont < ShaderInformation->Attributes.size(); ++cont )
        {
        if ( ShaderInformation->Attributes[cont].Name == Name )
            return ShaderAttributeHandle ( cont );
        }
	//LOG_ERROR ( "Invalid attribute '%s' for shader", Name.c_str() );
    return ShaderAttributeHandle::Invalid;
    }

void GetShaderInformation ( const ShaderHandle Handle, ShaderInformation &Information )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];
    for ( unsigned cont = 0; cont < ShaderInformation->Uniforms.size(); ++cont )
        {
        ShaderInformation::ShaderUniformInformation Info;
        Info.Name = ShaderInformation->Uniforms[cont].Name;
        Info.Type = ShaderInformation->Uniforms[cont].Type;
        Info.Handle = ShaderUniformHandle ( cont );
        Information.Uniforms.push_back ( Info );
        }
    for ( unsigned cont = 0; cont < ShaderInformation->Attributes.size(); ++cont )
        {
        ShaderInformation::ShaderAttributeInformation Info;
        Info.Name = ShaderInformation->Attributes[cont].Name;
        Info.Type = ShaderInformation->Attributes[cont].Type;
        Info.Handle = ShaderAttributeHandle ( cont );
        Information.Attributes.push_back ( Info );
        }
	Information.AttachedShaderObjects = ShaderInformation->AttachedShaderObjects;
    Information.Handle = Handle;
    }

bool DetectUniformsAndAttributes ( GLuint OpenGLID, std::vector <UniformInfo> &Uniforms, std::vector <AttributeInfo> &Attributes )
    {
    GLint UniformCount, MaxUniformNameLength, UniformBlockCount;
    GLint AttributeCount, MaxAttributeNameLength;
    bool Result = false;

    glGetProgramiv ( OpenGLID, GL_ACTIVE_UNIFORMS, &UniformCount );
    glGetProgramiv ( OpenGLID, GL_ACTIVE_ATTRIBUTES, &AttributeCount );
    glGetProgramiv ( OpenGLID, GL_ACTIVE_UNIFORM_BLOCKS, &UniformBlockCount );

    if ( !CheckError() )
        return false;
    if ( ( UniformCount == 0 ) && ( AttributeCount == 0 ) && ( UniformBlockCount == 0 ) )
        return true;

    glGetProgramiv ( OpenGLID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MaxUniformNameLength );
    glGetProgramiv ( OpenGLID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &MaxAttributeNameLength );
    if ( !CheckError() )
        return false;

    size_t NameLength = std::max ( MaxUniformNameLength, MaxAttributeNameLength );
    char *Name = new char [NameLength];

    Uniforms.clear();
    for ( int cont = 0; cont < UniformBlockCount; ++cont )
        {
        GLint Location;

        glGetActiveUniformBlockName ( OpenGLID, cont, ( GLsizei ) NameLength, nullptr, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( strncmp ( "gl_", Name, 3 ) == 0 )
            continue;

        Location = glGetUniformBlockIndex ( OpenGLID, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( Location == -1 )
            {
            LOG_ERROR ( "Unable to get location for uniform block '%s'", Name );
            continue;
            }

        UniformInfo NewUniform;
        NewUniform.Name.assign ( Name );
        NewUniform.Type = ShaderUniformType::Block;
        NewUniform.OpenGLID = Location;
        Uniforms.push_back ( NewUniform );
        }

    for ( int cont = 0; cont < UniformCount; ++cont )
        {
        GLenum GLType;
        GLint Location;
        GLint UniformSize;
        ShaderUniformType Type;

        glGetActiveUniform ( OpenGLID, cont, ( GLsizei ) NameLength, nullptr, &UniformSize, &GLType, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( strncmp ( "gl_", Name, 3 ) == 0 )
            continue;

        Location = glGetUniformLocation ( OpenGLID, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( Location == -1 )
            {
            // TODO Check if this is part of an uniform block
            LOG_ERROR ( "Unable to get location for uniform '%s'", Name );
            continue;
            }

        Type = TranslateOpenGLUniformType ( GLType );
        UniformInfo NewUniform;
        NewUniform.Name.assign ( Name );
        NewUniform.Type = Type;
        NewUniform.OpenGLID = Location;
        Uniforms.push_back ( NewUniform );
        }

    Attributes.clear();
    for ( int cont = 0; cont < AttributeCount; ++cont )
        {
        GLenum GLType;
        GLint Location;
        GLint AttributeSize;
        ShaderAttributeType Type;

        glGetActiveAttrib ( OpenGLID, cont, ( GLsizei ) NameLength, nullptr, &AttributeSize, &GLType, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( strncmp ( "gl_", Name, 3 ) == 0 )
            continue;

        Location = glGetAttribLocation ( OpenGLID, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( Location == -1 )
            {
            LOG_ERROR ( "Unable to get location for attribute '%s'", Name );
            goto cleanup;
            }

        Type = TranslateOpenGLUniformType ( GLType );
        AttributeInfo NewAttribute;
        NewAttribute.Name.assign ( Name );
        NewAttribute.Type = Type;
        NewAttribute.OpenGLID = Location;
        NewAttribute.Enabled = false;
        Attributes.push_back ( NewAttribute );
        }
    Result = true;

cleanup:
    delete[] Name;
    return Result;
    }
}
}
