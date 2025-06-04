#include "Renderer.h"

Renderer::Renderer(Shader &shader)
:shader(shader)
{
	this->initRenderData();
}

void Renderer::Delete()
{
	vao.Delete();
    customVAO.Delete();

    glDeleteVertexArrays(1, &s_Data.QuadVA);
    glDeleteBuffers(1, &s_Data.QuadVB);
    glDeleteBuffers(1, &s_Data.QuadIB);

    glDeleteTextures(1, &s_Data.WhiteTexture);

    delete[] s_Data.QuadBuffer;
}

void Renderer::DrawIndexedTexture(Texture &texture, Texture &palette, const glm::vec2& position, const glm::vec2& size, float rotate, const glm::vec4& color)
{
    this->shader.Use();
    this->shader.SetInteger("useTexture", 1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->shader.SetMatrix4("model", model);

    // render textured quad
    this->shader.SetVector4f("spriteColor", color);

    this->shader.SetInteger("image", 0);
    this->shader.SetInteger("Palette", 1);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glActiveTexture(GL_TEXTURE1);
    palette.Bind();

    this->vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->vao.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void Renderer::DrawIndexedTextureAtlas(Texture &texture, Texture &palette, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, bool flipped,bool flippedVert, const glm::vec4& color)
{
    float x = clipPos.x;
    float y = clipPos.y;
    float width = clipSize.x;
    float height = clipSize.y;
    
    if(rotated){
        std::swap(width, height);
    }
    
    float texLeft = x / texture.Width;
    float texRight = (x + width) / texture.Width;
    float texTop = y / texture.Height;
    float texBottom = (y + height) / texture.Height;
    
    if (flipped) {
        if(rotated)
            std::swap(texBottom, texTop);
        else
            std::swap(texLeft, texRight);
        
    }
    
    float vertices[] = {
        // pos      // tex
        0.0f, 0.0f, texLeft,  texTop,
        1.0f, 0.0f, texRight, texTop,
        0.0f, 1.0f, texLeft,  texBottom,
        1.0f, 0.0f, texRight, texTop,
        1.0f, 1.0f, texRight, texBottom,
        0.0f, 1.0f, texLeft,  texBottom
    };

    //     float vertices[] = {
    //     // pos                       // tex
    //     0.0f, 0.0f,      flipped ? texRight : texLeft, texTop,
    //     1.0f, 0.0f,      flipped ? texLeft : texRight, texTop,
    //     0.0f, 1.0f,      flipped ? (x + width) / texture.Width : x / texture.Width, (y + height) / texture.Height,
    //     1.0f, 0.0f,      flipped ? x / texture.Width : (x + width) / texture.Width, y / texture.Height,
    //     1.0f, 1.0f,      flipped ? x / texture.Width : (x + width) / texture.Width, (y + height) / texture.Height,
    //     0.0f, 1.0f,      flipped ? (x + width) / texture.Width : x / texture.Width, (y + height) / texture.Height
    // };

    this->customVAO.Bind();

    VBO vbo(vertices, sizeof(vertices));

    this->customVAO.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);

    this->customVAO.Unbind();
    vbo.Unbind();

    vbo.Delete();

    this->shader.Use();
    this->shader.SetInteger("useTexture", 1);
    glm::mat4 model = glm::mat4(1.0f);

    float shearX = 0.0f;
    float shearY = 0.0f;

    model = glm::translate(model, glm::vec3(position, 0.0f));

   
    glm::mat4 shear = glm::mat4(1.0f);
    shear[1][0] = shearX;
    shear[0][1] = shearY;

    if(flippedVert)
        model *= shear;
    

    if(rotated && flippedVert)
    {
        if(flipped)
            model = glm::translate(model, glm::vec3(clipSize.x, clipSize.y, 0));
        else
            model = glm::translate(model, glm::vec3(0, clipSize.y, 0));

        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    } else if(rotated)
    {
        model = glm::translate(model, glm::vec3(0, clipSize.y, 0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    } else if(flippedVert){
        model = glm::translate(model, glm::vec3(clipSize.x,clipSize.y, 0));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);

    shader.SetInteger("image", 0);
    shader.SetInteger("Palette", 1);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glActiveTexture(GL_TEXTURE1);
    palette.Bind();
    
    this->customVAO.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->customVAO.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Renderer::DrawTextureAtlas(Texture &texture, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, const glm::vec4& color)
{
    float x = clipPos.x;
    float y = clipPos.y;
    float width = clipSize.x;
    float height = clipSize.y;
    if(rotated){
        std::swap(width, height);
    }

    float vertices[] = {
        // pos                       // tex
        0.0f, 0.0f,      x / texture.Width, y / texture.Height,
        1.0f, 0.0f,      (x + width) / texture.Width, y / texture.Height,
        0.0f, 1.0f,      x / texture.Width, (y + height) / texture.Height,

        1.0f, 0.0f,      (x + width) / texture.Width, y / texture.Height,
        1.0f, 1.0f,         (x + width) / texture.Width, (y + height) / texture.Height,
        0.0f, 1.0f,      x / texture.Width, (y + height) / texture.Height
    };

    this->customVAO.Bind();

    VBO vbo(vertices, sizeof(vertices));

    this->customVAO.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);

    this->customVAO.Unbind();
    vbo.Unbind();

    vbo.Delete();

    this->shader.Use();
    this->shader.SetInteger("useTexture", 1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    if(rotated)
    {
        model = glm::translate(model, glm::vec3(0, clipSize.y, 0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();
    
    this->customVAO.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->customVAO.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawTexture(Texture &texture, const glm::vec2& position, const glm::vec2& size, float rotate, const glm::vec4& color)
{
	this->shader.Use();
    this->shader.SetInteger("useTexture", 1);
	glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->shader.SetMatrix4("model", model);

    // render textured quad
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    this->vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->vao.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawTexture(Texture *texture, const glm::vec2& position, const glm::vec2& size, float rotate, const glm::vec4& color)
{
    this->shader.Use();
    this->shader.SetInteger("useTexture", 1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture->Bind();
    
    this->vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->vao.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawRect(const glm::vec2& position, const glm::vec2& size, float rotate, const glm::vec4& color)
{
    this->shader.Use();
    this->shader.SetInteger("useTexture", 0);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);
    
    this->vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->vao.Unbind();
}

void Renderer::DrawTexture(Texture &texture, float x1, float y1, float x2, float y2, float rotate, const glm::vec4& color)
{
    this->shader.Use();
    this->shader.SetInteger("useTexture", 1);

    glm::mat4 model = glm::mat4(1.0f);

    // Calculate the position and size from the coordinates
    glm::vec2 position = glm::vec2((x1 + x2) / 2.0f, (y1 + y2) / 2.0f);
    glm::vec2 size = glm::vec2(x2 - x1, y2 - y1);

    model = glm::translate(model, glm::vec3(position, 0.0f)); // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->shader.SetMatrix4("model", model);

    // render textured quad
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    this->vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->vao.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawTexture(Texture &texture, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const glm::vec4& color)
{

    float vertices[] = { 
        // pos      // tex
        x1, y1, 0.0f, 1.0f, //top left
        x3, y3, 1.0f, 0.0f, //bottom right
        x4, y4, 0.0f, 0.0f, //bottom left

        x1, y1, 0.0f, 1.0f,
        x2, y2, 1.0f, 1.0f, //top right
        x3, y3, 1.0f, 0.0f
    };

    this->customVAO.Bind();

    VBO vbo(vertices, sizeof(vertices));

    this->customVAO.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);

    this->customVAO.Unbind();
    vbo.Unbind();

    vbo.Delete();

    this->shader.Use();
    this->shader.SetInteger("useTexture", 1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();
    
    this->customVAO.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->customVAO.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawTexture(Texture &texture, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float textureCropBottom, const glm::vec4& color)
{

    float vertices[] = { 
        // pos      // tex
        x1, y1, 0.0f, 1.0f - textureCropBottom, //top left
        x3, y3, 1.0f, 0.0f + textureCropBottom, //bottom right
        x4, y4, 0.0f, 0.0f + textureCropBottom, //bottom left

        x1, y1, 0.0f, 1.0f - textureCropBottom,
        x2, y2, 1.0f, 1.0f - textureCropBottom, //top right
        x3, y3, 1.0f, 0.0f + textureCropBottom
    };

    this->customVAO.Bind();

    VBO vbo(vertices, sizeof(vertices));

    this->customVAO.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);

    this->customVAO.Unbind();
    vbo.Unbind();

    vbo.Delete();

    this->shader.Use();
    this->shader.SetInteger("useTexture", 1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();
    
    this->customVAO.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->customVAO.Unbind();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawShadow(const glm::vec2& segmentA, const glm::vec2& segmentB, const glm::vec2& light_position,  const glm::vec4& color)
{
    float vertices[] = { 
        // pos      // tex
        segmentA.x, segmentA.y, 0.0f, 1.0f, //top left
        (segmentB.x + 100*(segmentB.x - light_position.x)), (segmentB.y + 100*(segmentB.y - light_position.y)), 1.0f, 0.0f, //bottom right
        segmentB.x, segmentB.y, 0.0f, 0.0f, //bottom left

        segmentA.x, segmentA.y, 0.0f, 1.0f,
        (segmentA.x + 100*(segmentA.x - light_position.x)), (segmentA.y + 100*(segmentA.y - light_position.y)), 1.0f, 1.0f, //top right
        (segmentB.x + 100*(segmentB.x - light_position.x)), (segmentB.y + 100*(segmentB.y - light_position.y)), 1.0f, 0.0f
    };

    this->customVAO.Bind();

    VBO vbo(vertices, sizeof(vertices));

    this->customVAO.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);

    this->customVAO.Unbind();
    vbo.Unbind();

    vbo.Delete();

    this->shader.Use();
    this->shader.SetInteger("useTexture", 0);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f)); // last scale

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);
    
    this->customVAO.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->customVAO.Unbind();

}


void Renderer::initRenderData()
{
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    this->vao.Bind();

    VBO vbo(vertices, sizeof(vertices));

    this->vao.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);

    this->vao.Unbind();
    vbo.Unbind();

    vbo.Delete();
}

void Renderer::initBatchRenderData()
{
    this->isBatch = true;
    
    this->shader.Use();
    auto loc = glGetUniformLocation(this->shader.ID, "u_Textures");
    int samplers[32];
    for(int i = 0; i < 32; i++)
        samplers[i] = i;
    glUniform1iv(loc, 32, samplers);

    s_Data.QuadBuffer = new Vertex[MaxVertexCount];

    glGenVertexArrays(1, &s_Data.QuadVA);
    glBindVertexArray(s_Data.QuadVA);

    glGenBuffers(1, &s_Data.QuadVB);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVB);
    glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexIndex));

    uint32_t indices[MaxIndexCount];
    uint32_t offset = 0;
    for(size_t i = 0; i < MaxIndexCount; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }

    glGenBuffers(1, &s_Data.QuadIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenTextures(1, &s_Data.WhiteTexture);
    glBindTexture(GL_TEXTURE_2D, s_Data.WhiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    uint32_t color = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    s_Data.TextureSlots[0] = s_Data.WhiteTexture;
    for(size_t i = 1; i < MaxTextures; i++)
        s_Data.TextureSlots[i] = 0;
}


const Stats& Renderer::GetStats()
{
    return s_Data.RenderStats;
}

void Renderer::ResetStats()
{
    s_Data.RenderStats = {};
}

void Renderer::BeginBatch()
{
    s_Data.QuadBufferPtr = s_Data.QuadBuffer;
}

void Renderer::EndBatch()
{
    GLsizeiptr size = (uint8_t*)s_Data.QuadBufferPtr - (uint8_t*)s_Data.QuadBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data.QuadBuffer);
}

void Renderer::Flush()
{
    this->shader.Use();
    for(uint32_t i = 0; i < s_Data.TextureSlotIndex; i++){
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, s_Data.TextureSlots[i]);
    }

    glBindVertexArray(s_Data.QuadVA);
    glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);
    s_Data.RenderStats.DrawCount++;

    s_Data.IndexCount = 0;
    s_Data.TextureSlotIndex = 1;
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
    if(s_Data.IndexCount >= MaxIndexCount)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

    float textureIndex = 0.0f;

        float sinRot = approxsin(rotation);
        float cosRot = glm::sqrt(1.0f - sinRot * sinRot);
        glm::vec2 center = position + size * 0.5f;

        s_Data.QuadBufferPtr->Position = { (position.x - center.x) * cosRot - (position.y - center.y) * sinRot + center.x, 
        (position.x - center.x) * sinRot + (position.y - center.y) * cosRot + center.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
        s_Data.QuadBufferPtr->TexIndex = textureIndex;
        s_Data.QuadBufferPtr++;

        s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x) * cosRot - (position.y - center.y) * sinRot + center.x, 
        (position.x + size.x - center.x) * sinRot + (position.y - center.y) * cosRot + center.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoords = { 1.0f, 0.0f };
        s_Data.QuadBufferPtr->TexIndex = textureIndex;
        s_Data.QuadBufferPtr++;

        s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x) * cosRot - (position.y + size.y - center.y) * sinRot + center.x, 
        (position.x + size.x - center.x) * sinRot + (position.y + size.y - center.y) * cosRot + center.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoords = { 1.0f, 1.0f };
        s_Data.QuadBufferPtr->TexIndex = textureIndex;
        s_Data.QuadBufferPtr++;

        s_Data.QuadBufferPtr->Position = { (position.x - center.x) * cosRot - (position.y + size.y - center.y) * sinRot + center.x, 
        (position.x - center.x) * sinRot + (position.y + size.y - center.y) * cosRot + center.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
        s_Data.QuadBufferPtr->TexIndex = textureIndex;
        s_Data.QuadBufferPtr++;

        s_Data.IndexCount += 6;
        s_Data.RenderStats.QuadCount++;
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, const glm::vec3& cameraPosition, float parallaxFactor)
{
    if(s_Data.IndexCount >= MaxIndexCount)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

    glm::vec2 parallaxOffset = glm::vec2(cameraPosition.x * parallaxFactor, cameraPosition.y * parallaxFactor);

    float textureIndex = 0.0f;

    float sinRot = approxsin(rotation);
    float cosRot = glm::sqrt(1.0f - sinRot * sinRot);
    glm::vec2 center = position + size * 0.5f;

    s_Data.QuadBufferPtr->Position = { (position.x - center.x - parallaxOffset.x) * cosRot - (position.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x - center.x - parallaxOffset.x) * sinRot + (position.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x - parallaxOffset.x) * cosRot - (position.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x + size.x - center.x - parallaxOffset.x) * sinRot + (position.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 1.0f, 0.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x - parallaxOffset.x) * cosRot - (position.y + size.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x + size.x - center.x - parallaxOffset.x) * sinRot + (position.y + size.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 1.0f, 1.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x - center.x - parallaxOffset.x) * cosRot - (position.y + size.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x - center.x - parallaxOffset.x) * sinRot + (position.y + size.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RenderStats.QuadCount++;
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, uint32_t textureID)
{
    if(s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > 31)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

    constexpr glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

    float textureIndex = 0.0f;
    for(uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if(s_Data.TextureSlots[i] == textureID)
        {
            textureIndex = (float)i;
            break;
        }
    }

    if(textureIndex == 0.0f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = textureID;
        s_Data.TextureSlotIndex++;
    }

    float sinRot = approxsin(rotation);
    float cosRot = glm::sqrt(1.0f - sinRot * sinRot);
    glm::vec2 center = position + size * 0.5f;

    s_Data.QuadBufferPtr->Position = { (position.x - center.x) * cosRot - (position.y - center.y) * sinRot + center.x, 
    (position.x - center.x) * sinRot + (position.y - center.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x) * cosRot - (position.y - center.y) * sinRot + center.x, 
    (position.x + size.x - center.x) * sinRot + (position.y - center.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 1.0f, 0.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x) * cosRot - (position.y + size.y - center.y) * sinRot + center.x, 
    (position.x + size.x - center.x) * sinRot + (position.y + size.y - center.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 1.0f, 1.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x - center.x) * cosRot - (position.y + size.y - center.y) * sinRot + center.x, 
    (position.x - center.x) * sinRot + (position.y + size.y - center.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RenderStats.QuadCount++;
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, uint32_t textureID, const glm::vec3& cameraPosition, float parallaxFactor)
{
    if(s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > 31)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

    constexpr glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

    float textureIndex = 0.0f;
    for(uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if(s_Data.TextureSlots[i] == textureID)
        {
            textureIndex = (float)i;
            break;
        }
    }

    if(textureIndex == 0.0f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = textureID;
        s_Data.TextureSlotIndex++;
    }

    glm::vec2 parallaxOffset = glm::vec2(cameraPosition.x * parallaxFactor, cameraPosition.y * parallaxFactor);
    float sinRot = approxsin(rotation);
    float cosRot = glm::sqrt(1.0f - sinRot * sinRot);
    glm::vec2 center = position + size * 0.5f;

    s_Data.QuadBufferPtr->Position = { (position.x - center.x - parallaxOffset.x) * cosRot - (position.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x - center.x - parallaxOffset.x) * sinRot + (position.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x - parallaxOffset.x) * cosRot - (position.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x + size.x - center.x - parallaxOffset.x) * sinRot + (position.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 1.0f, 0.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x + size.x - center.x - parallaxOffset.x) * cosRot - (position.y + size.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x + size.x - center.x - parallaxOffset.x) * sinRot + (position.y + size.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 1.0f, 1.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { (position.x - center.x - parallaxOffset.x) * cosRot - (position.y + size.y - center.y - parallaxOffset.y) * sinRot + center.x, 
    (position.x - center.x - parallaxOffset.x) * sinRot + (position.y + size.y - center.y - parallaxOffset.y) * cosRot + center.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RenderStats.QuadCount++;
}

void Renderer::DrawOutline(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, float lineThickness)
{
    // Draw the top line
    DrawQuad(
        { position.x, position.y },
        { size.x, lineThickness },
        rotation,
        color
    );

    // Draw the bottom line
    DrawQuad(
        { position.x, position.y + size.y - lineThickness },
        { size.x, lineThickness },
        rotation,
        color
    );

    // Draw the left line
    DrawQuad(
        { position.x, position.y },
        { lineThickness, size.y },
        rotation,
        color
    );

    // Draw the right line
    DrawQuad(
        { position.x + size.x - lineThickness, position.y },
        { lineThickness, size.y },
        rotation,
        color
    );
}

void Renderer::DrawQuadAtlas(Texture& texture, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, const glm::vec4& color, const glm::vec2& size)
{
    if (s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > 31)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

    float x = clipPos.x;
    float y = clipPos.y;
    float width = clipSize.x;
    float height = clipSize.y;

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if (s_Data.TextureSlots[i] == texture.ID)
        {
            textureIndex = static_cast<float>(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture.ID;
        s_Data.TextureSlotIndex++;
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    if (rotated)
    {
        std::swap(width, height);
        model = glm::translate(model, glm::vec3(0, clipSize.y, 0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glm::vec4 topLeft = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 topRight = model * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 bottomRight = model * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 bottomLeft = model * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    s_Data.QuadBufferPtr->Position = { topLeft.x, topLeft.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { x / texture.Width, y / texture.Height };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { topRight.x, topRight.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { (x + width) / texture.Width, y / texture.Height };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { bottomRight.x, bottomRight.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { (x + width) / texture.Width, (y + height) / texture.Height };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { bottomLeft.x, bottomLeft.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { x / texture.Width, (y + height) / texture.Height };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RenderStats.QuadCount++;
}

void Renderer::DrawFlippedQuadAtlas(Texture& texture, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, bool flipX, bool flipY, const glm::vec4& color)
{
    if (s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > 31)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

    float x = clipPos.x;
    float y = clipPos.y;
    float width = clipSize.x;
    float height = clipSize.y;

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if (s_Data.TextureSlots[i] == texture.ID)
        {
            textureIndex = static_cast<float>(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture.ID;
        s_Data.TextureSlotIndex++;
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    if (rotated)
    {
        std::swap(width, height);
        model = glm::translate(model, glm::vec3(0, clipSize.y, 0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    glm::vec4 topLeft = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 topRight = model * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 bottomRight = model * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 bottomLeft = model * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    float u0 = x / texture.Width;
    float v0 = y / texture.Height;
    float u1 = (x + width) / texture.Width;
    float v1 = (y + height) / texture.Height;

    if (flipX)
    {
        if(rotated)
            std::swap(v0, v1);
        else
            std::swap(u0, u1);
    }
    if (flipY) std::swap(v0, v1);

    s_Data.QuadBufferPtr->Position = { topLeft.x, topLeft.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { u0, v0 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { topRight.x, topRight.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { u1, v0 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { bottomRight.x, bottomRight.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { u1, v1 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { bottomLeft.x, bottomLeft.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoords = { u0, v1 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RenderStats.QuadCount++;
}