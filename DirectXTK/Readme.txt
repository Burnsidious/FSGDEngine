--------------------------------
DirectXTK - the DirectX Tool Kit
--------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

May 31, 2012

This package contains the "DirectX Tool Kit", a collection of helper classes for
writing Direct3D 11 code for Metro style apps, Windows 8 Desktop, and Windows 7 'classic'
applications in C++.

This code is designed to build with either Visual Studio 11 which includes the 
Windows 8.0 SDK or Visual Studio 2010 with the standalone Windows 8.0 SDK installed 
using the "Platform Toolset" set to use the Windows 8.0 SDK. See the Visual C++ Team Blog 
<http://blogs.msdn.com/b/vcblog/archive/2012/03/25/10287354.aspx>. It makes use of 
the DirectXMath library and optionally the DXGI 1.2 headers from the Windows 8.0 SDK 
as well as other headers.

These components are designed to work without requiring any content from the DirectX SDK. For details,
see "Where is the DirectX SDK"? <http://msdn.microsoft.com/en-us/library/ee663275.aspx>

Inc\
    Public Header Files:

    SpriteBatch.h - simple & efficient 2D sprite rendering
    SpriteFont.h - bitmap based text rendering
    Effects.h - set of built-in shaders for common rendering tasks
    GeometricPrimitive.h - draws basic shapes such as cubes and spheres
    CommonStates.h - factory providing commonly used D3D state objects
    VertexTypes.h - structures for commonly used vertex data formats
    DDSTextureLoader.h - light-weight DDS file texture loader
    WICTextureLoader.h - WIC-based image file texture loader

Src\
    DirectXTK source files and internal implementation headers

MakeSpriteFont\
    Command line tool used to generate binary resources for use with SpriteFont

All content and source code for this package are bound to the Microsoft Public License (Ms-PL)
<http://www.microsoft.com/en-us/openness/licenses.aspx#MPL>.

http://go.microsoft.com/fwlink/?LinkId=248929


-----------
SpriteBatch
-----------

This is a native D3D11 implementation of the SpriteBatch helper from XNA Game 
Studio, providing identical functionality and API.

During initialization:

    std::unique_ptr<SpriteBatch> spriteBatch(new SpriteBatch(deviceContext));

Simple drawing:

    spriteBatch->Begin();
    spriteBatch->Draw(texture, XMFLOAT2(x, y));
    spriteBatch->End();

The Draw method has many overloads with parameters controlling:

    - Specify position as XMFLOAT2, XMVECTOR or RECT
    - Optional source rectangle for drawing just part of a sprite sheet
    - Tint color
    - Rotation (in radians)
    - Origin point (position, scaling and rotation are relative to this)
    - Scale
    - SpriteEffects enum (for horizontal or vertical mirroring)
    - Layer depth (for sorting)

Sorting:

    The first parameter to SpriteBatch::Begin is a SpriteSortMode enum. For 
    most efficient rendering, use SpriteSortMode_Deferred (which batches up 
    sprites, then submits them all to the GPU during the End call), and 
    manually draw everything in texture order. If it is not possible to draw 
    in texture order, the second most efficient approach is to use 
    SpriteSortMode_Texture, which will automatically sort on your behalf.

    When drawing scenes with multiple depth layers, SpriteSortMode_BackToFront 
    or SpriteSortMode_FrontToBack will sort by the layerDepth parameter 
    specified to each Draw call.

    SpriteSortMode_Immediate disables all batching, submitting a separate D3D 
    draw call for each sprite. This is expensive, but convenient in rare cases 
    when you need to set shader constants differently per sprite.

    Multiple SpriteBatch instances are lightweight. It is reasonable to 
    create several, Begin them at the same time with different sort modes, 
    submit sprites to different batches in arbitrary orders as you traverse a 
    scene, then End the batches in whatever order you want these groups of 
    sprites to be drawn.

Custom render states:

    By default SpriteBatch uses premultiplied alpha blending, no depth buffer, 
    counter clockwise culling, and linear filtering with clamp texture 
    addressing. You can change this (if for instance you do not wish to use 
    premultiplied alpha) by passing custom state objects to SpriteBatch::Begin. 
    Pass null for any parameters that should use their default state.

    To use SpriteBatch with a custom pixel shader (handy for 2D postprocessing 
    effects such as bloom or blur) or even a custom vertex shader, use the 
    setCustomShaders parameter to specify a state setting callback function:

    spriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr, [=]
    {
        deviceContext->PSSetShader(...);
        deviceContext->PSSetConstantBuffers(...);
        deviceContext->PSSetShaderResources(...);
    });

    SpriteBatch automatically sets pixel shader resource #0 to the texture 
    specified by each Draw call, so you only need to call PSSetResources for 
    any additional textures required by your shader.

    SpriteBatch::Begin also has a transformMatrix parameter, which can be used 
    for global transforms such as scaling or translation of an entire scene.

Threading model:

    Creation is fully asynchronous, so you can instantiate multiple SpriteBatch 
    instances at the same time on different threads. Each SpriteBatch instance 
    only supports drawing from one thread at a time, but you can simultaneously 
    submit sprites on multiple threads if you create a separate SpriteBatch 
    instance per D3D11 deferred context.

Further reading:

    http://www.shawnhargreaves.com/blogindex.html#spritebatch
    http://blogs.msdn.com/b/shawnhar/archive/2010/06/18/spritebatch-and-renderstates-in-xna-game-studio-4-0.aspx
    http://www.shawnhargreaves.com/blogindex.html#premultipliedalpha



----------
SpriteFont
----------

This is a native D3D11 implementation of a bitmap font renderer, similar to the 
SpriteFont type from XNA Game Studio, plus a command line tool (MakeSpriteFont) 
for building fonts into bitmap format. It is less fully featured than Direct2D 
and DirectWrite, but may be useful for those who want something simpler and 
lighter weight.

At build time:

    MakeSpriteFont.exe "Comic Sans" myfile.spritefont /FontSize:16

During initialization:

    std::unique_ptr<SpriteBatch> spriteBatch(new SpriteBatch(deviceContext));
    std::unique_ptr<SpriteFont> spriteFont(new SpriteFont(device, L"myfile.spritefont"));

Simple drawing:

    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(), L"Hello, world!", XMFLOAT2(x, y));
    spriteBatch->End();

The Draw method has several overloads with parameters controlling color, 
rotation, origin point, scaling, horizontal or vertical mirroring, and layer 
depth. These work the same way as the equivalent SpriteBatch::Draw parameters.

SpriteFont has three constructors:

    - Pass a filename string to read a binary file created by MakeSpriteFont
    - Pass a buffer containing a MakeSpriteFont binary that was already loaded some other way
    - Pass an array of Glyph structs if you prefer to entirely bypass MakeSpriteFont

If you try to draw or call MeasureString with a character that is not included in 
the font, by default you will get an exception. Use SetDefaultCharacter to 
specify some other character that will be automatically substituted in place of 
any that are missing.

This implementation supports sparse fonts, so if you are localizing into 
languages such as Chinese, Japanese, or Korean, you can build a spritefont 
including only the specific characters needed by your program. This is usually a 
good idea for CJK languages, as a complete CJK character set is too large to fit 
in a Direct3D texture! (if you need full CJK support, Direct2D or DirectWrite would
be a better choice). SpriteFont does not support combining characters or
right-to-left (RTL) layout, so it will not work for languages with complex layout
requirements such as Arabic or Thai.

The MakeSpriteFont tool can process any TrueType font that is installed on your 
system (using GDI+ to rasterize them into a bitmap) or it can import character 
glyphs from a specially formatted bitmap file. This latter option allows you to 
create multicolored fonts, drawing special effects such as gradients or drop 
shadows directly into your glyph textures. Characters should be arranged in a 
grid ordered from top left to bottom right. Monochrome fonts should use white for 
solid areas and black for transparent areas. To include multicolored characters, 
add an alpha channel to the bitmap and use that to control which parts of each 
character are solid. The spaces between characters and around the edges of the 
grid should be filled with bright pink (red=255, green=0, blue=255). It doesn't 
matter if your grid includes lots of wasted space, because the converter will 
rearrange characters, packing everything as tightly as possible.

Commandline options for the MakeSpriteFont tool:

    /CharacterRegion:<region>
        Specifies which Unicode codepoints to include in the font. Can be 
        repeated to include more than one region. If not specified, the default 
        ASCII range (32-126) is used. Examples:
            /CharacterRegion:a-z
            /CharacterRegion:0x1200-0x1250
            /CharacterRegion:0x1234

    /DefaultCharacter:<value>
        Fallback character substituted in place of codepoints that are not 
        included in the font. If zero, missing characters throw exceptions.

    /FontSize:<value>
    /FontStyle:<value>
        Size and style (bold or italic) for TrueType fonts. Ignored when 
        converting a bitmap font.

    /LineSpacing:<value>
    /CharacterSpacing:<value>
        Spacing overrides. Zero is default spacing, negative closer together, 
        positive further apart.

    /TextureFormat:<value>
        What format should the output texture be? Options:
            Auto
                The default. Chooses between CompressedMono and Rgba32 depending 
                on whether the font data is monochromatic or multicolored.
            Rgba32
                High quality and supports multicolored fonts, but wastes space.
            Bgra4444
                Good choice for color fonts on Metro platforms, but this format 
                requires the DirectX 11.1 Runtime and a WDDM 1.2 driver.
            CompressedMono
                The smallest format, and works on all D3D platforms, but it only 
                supports monochromatic font data. This uses a special BC2 
                encoder: see comments in SpriteFontWriter.cs for details.

    /NoPremultiply
        By default, font textures use premultiplied alpha format. Pass this flag 
        if you want interpolative alpha instead.

    /DebugOutputSpriteSheet:<filename>
        Dumps the generated texture to a bitmap file (useful when debugging the 
        MakeSpriteFont tool, not so much if you are just trying to use it).

Further reading:

    http://blogs.msdn.com/b/shawnhar/archive/2007/04/26/bitmap-fonts-in-xna.aspx
    http://create.msdn.com/en-US/education/catalog/utility/bitmap_font_maker



-------
Effects
-------

This is a native D3D11 implementation of the five built-in effects from XNA 
Game Studio, providing identical functionality and API:

    - BasicEffect supports texture mapping, vertex coloring, directional lighting, and fog
    - AlphaTestEffect supports per-pixel alpha testing
    - DualTextureEffect supports two layer multitexturing (for lightmaps or detail textures)
    - EnvironmentMapEffect supports cubic environment mapping
    - SkinnedEffect supports skinned animation

During initialization:

    std::unique_ptr<BasicEffect> effect(new BasicEffect(device));

Set effect parameters:

    effect->SetWorld(world);
    effect->SetView(view);
    effect->SetProjection(projection);

    effect->SetTexture(cat);
    effect->SetTextureEnabled(true);

    effect->EnableDefaultLighting();

Draw using the effect:

    effect->Apply(deviceContext);

    deviceContext->IASetInputLayout(...);
    deviceContext->IASetVertexBuffers(...);
    deviceContext->IASetIndexBuffer(...);
    deviceContext->IASetPrimitiveTopology(...);

    deviceContext->DrawIndexed(...);

To create an input layout matching the effect vertex shader input signature:

    // First, configure effect parameters the way you will be using it. Turning
    // lighting, texture map, or vertex color on/off alters which vertex shader
    // is used, so GetVertexShaderBytecode will return a different blob after
    // you alter these parameters. If you create an input layout using a
    // BasicEffect that had lighting disabled, but then later enable lighting,
    // that input layout will no longer match as it will not include the
    // now-necessary normal vector.

    void const* shaderByteCode;
    size_t byteCodeLength;

    effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    device->CreateInputLayout(VertexPositionNormalTexture::InputElements,
                              VertexPositionNormalTexture::InputElementCount,
                              shaderByteCode, byteCodeLength,
                              pInputLayout);

Threading model:

    Creation is fully asynchronous, so you can instantiate multiple effect 
    instances at the same time on different threads. Each instance only 
    supports drawing from one thread at a time, but you can simultaneously draw 
    on multiple threads if you create a separate effect instance per D3D11 
    deferred context.

Further reading:

    http://blogs.msdn.com/b/shawnhar/archive/2010/04/28/new-built-in-effects-in-xna-game-studio-4-0.aspx
    http://blogs.msdn.com/b/shawnhar/archive/2010/04/30/built-in-effects-permutations-and-performance.aspx
    http://blogs.msdn.com/b/shawnhar/archive/2010/04/25/basiceffect-optimizations-in-xna-game-studio-4-0.aspx
    http://blogs.msdn.com/b/shawnhar/archive/2008/08/22/basiceffect-a-misnomer.aspx
    http://blogs.msdn.com/b/shawnhar/archive/2010/08/04/dualtextureeffect.aspx
    http://blogs.msdn.com/b/shawnhar/archive/2010/08/09/environmentmapeffect.aspx



------------------
GeometricPrimitive
------------------

This is a helper for drawing simple geometric shapes:

    - Cube
    - Sphere
    - Cylinder
    - Torus
    - Teapot

During initialization:

    std::unique_ptr<GeometricPrimitive> shape(GeometricPrimitive::CreateTeapot(deviceContext));

Simple drawing:

    shape->Draw(world, view, projection, Colors::CornflowerBlue);

The draw method accepts an optional texture parameter, wireframe flag, and a 
callback function which can be used to override the default rendering state:

    shape->Draw(world, view, projection, Colors::White, catTexture, false, [=]
    {
        deviceContext->OMSetBlendState(...);
    });



------------
CommonStates
------------

The CommonStates class is a factory which simplifies setting the most common 
combinations of D3D rendering states.

During initialization:

    std::unique_ptr<CommonStates> states(new CommonStates(device));

Using this helper to set device state:

    deviceContext->OMSetBlendState(states->Opaque(), Colors::Black, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(states->DepthDefault(), 0);
    deviceContext->RSSetState(states->CullCounterClockwise());

    auto samplerState = states->LinearWrap();
    deviceContext->PSSetSamplers(0, 1, &samplerState);

Available states:

    ID3D11BlendState* Opaque();
    ID3D11BlendState* AlphaBlend();
    ID3D11BlendState* Additive();
    ID3D11BlendState* NonPremultiplied();

    ID3D11DepthStencilState* DepthNone();
    ID3D11DepthStencilState* DepthDefault();
    ID3D11DepthStencilState* DepthRead();

    ID3D11RasterizerState* CullNone();
    ID3D11RasterizerState* CullClockwise();
    ID3D11RasterizerState* CullCounterClockwise();
    ID3D11RasterizerState* Wireframe();

    ID3D11SamplerState* PointWrap();
    ID3D11SamplerState* PointClamp();
    ID3D11SamplerState* LinearWrap();
    ID3D11SamplerState* LinearClamp();
    ID3D11SamplerState* AnisotropicWrap();
    ID3D11SamplerState* AnisotropicClamp();



-----------
VertexTypes
-----------

VertexTypes.h defines these commonly used vertex data structures:

    - VertexPositionColor
    - VertexPositionTexture
    - VertexPositionNormal
    - VertexPositionColorTexture
    - VertexPositionNormalColor
    - VertexPositionNormalTexture
    - VertexPositionNormalColorTexture

Each type also provides a D3D11_INPUT_ELEMENT_DESC array which can be used to 
create a matching input layout, for example:

    device->CreateInputLayout(VertexPositionColorTexture::InputElements,
                              VertexPositionColorTexture::InputElementCount,
                              vertexShaderCode, vertexShaderCodeSize,
                              &inputLayout);



----------------
DDSTextureLoader
----------------

DDSTextureLoader.h contains a simple light-weight loader for .dds files. It supports
1D and 2D textures, texture arrays, cubemaps, volume maps, and mipmap levels. This
version performs no legacy conversions, so the .dds file must directly map to a
DXGI_FORMAT (i.e. legacy 24 bpp .dds files will fail to load). It does support both
'legacy' and 'DX10' extension file header format .dds files.

For a full-featured DDS file reader, writer, and texture processing pipeline see
the 'Texconv' sample and the 'DirectXTex' library

CreateDDSTextureFromFile and CreateDDSTextureFromMemory can be called requesting only
a 'texture' resource (i.e. textureView is null), requesting only a shader resource
view (i.e. texture is null), or both.

If maxsize is non-zero, then any mipmap level larger than the maxsize present in the
file will be ignored and only smaller mipmap levels (if present) will be used. This
can be used to scale the content at load-time. Note this requires the .dds file contain
mipmaps to retry with a smaller size.

If maxsize is 0 and the initial attempt to create the texture fails, it will attempt
to trim off any mipmap levels larger than the minimum required size for the given
device Feature Level and retry. Note this requires the .dds file contains mipmaps to
retry with a smaller size.

DDSTextureLoader will load BGR 5:6:5 and BGRA 5:5:5:1 DDS files, but these formats will
fail to create on a system with DirectX 11.0 Runtime. The DXGI 1.2 version of
DDSTextureLoader will load BGRA 4:4:4:4 DDS files using DXGI_FORMAT_B4G4R4A4_UNORM.
The DirectX 11.1 Runtime, DXGI 1.2 headers, and WDDM 1.2 drivers are required to
support 16bpp pixel formats on all feature levels.

Further reading:
    http://go.microsoft.com/fwlink/?LinkId=248926
    http://blogs.msdn.com/b/chuckw/archive/2011/10/28/directxtex.aspx



----------------
WICTextureLoader
----------------

WICTextureLoader.h contains a loader for BMP, JPEG, PNG, TIFF, GIF, HD Photo, and
other WIC-supported image formats. This performs any required pixel format conversions
or image resizing using WIC at load time as well.

NOTE: WICTextureLoader cannot load .TGA files unless the system has a 3rd party WIC
codec installed. You must use the DirectXTex library for TGA file format support
without relying on an add-on WIC codec.

CreateWICTextureFromFile and CreateWICTextureFromMemory can be called requesting only
a 'texture' resource (i.e. textureView is null), requesting only a shader resource
view (i.e. texture is null), or both.

If maxsize is non-zero, then if the image is larger than maxsize it will be resized
so that the largest dimension matches 'maxsize' (the other dimension will be derived
respecting the original aspect-ratio of the image).

If the maxsize is 0, then the maxsize will be implicitly set to the minimum required
supported texture size based on the device's current feature level (2k, 4k, 8k, or 16k).

If a Direct3D 11 context is provided and a shader resource view is requested (i.e.
textureView is non-null), then the texture will be created to support auto-generated
mipmaps and will have GenerateMips called on it before returning. If no context is
provided (i.e. d3dContext is null) or the pixel format is unsupported for auto-gen
mips by the current device, then the resulting texture will have only a single level.

This loader does not support array textures, 1D textures, 3D volume textures, or 
cubemaps. For these scenarios, use the .DDS file format and DDSTextureLoader instead.

The DXGI 1.2 version of WICTextureLoader will load 16bpp pixel images as 5:6:5 or
5:5:5:1 rather than expand them to 32bpp RGBA. The DirectX 11.1 Runtime, DXGI 1.2
headers, and WDDM 1.2 drivers are required to support 16bpp pixel formats on all
feature levels.

Further reading:
    http://go.microsoft.com/fwlink/?LinkId=248926
    http://blogs.msdn.com/b/chuckw/archive/2011/10/28/directxtex.aspx



---------------
RELEASE HISTORY
---------------

May 31, 2012
    Updated Metro project for Visual Studio 2012 Release Candidate changes
    Cleaned up x64 Debug configuration warnings and switched to use "_DEBUG" instead of "DEBUG"
    Minor fix for DDSTextureLoader's retry fallback that can happen with 10level9 feature levels

May 2, 2012
    Added SpriteFont implementation and the MakeSpriteFont utility

March 29, 2012
    WICTextureLoader updated with Windows 8 WIC native pixel formats

March 6, 2012
    Fix for too much temp memory used by WICTextureLoader
    Add separate Visual Studio 11 projects for Desktop vs. Metro builds

March 5, 2012
    Bug fix for SpriteBatch with batches > 2048

February 24, 2012
    Original release
