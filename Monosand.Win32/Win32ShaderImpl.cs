﻿using System.Numerics;

namespace Monosand.Win32;

internal class Win32ShaderImpl : GraphicsImplBase, IShaderImpl
{
    private Win32RenderContext context;
    private IntPtr shaderHandle;

    private Win32ShaderImpl(Win32RenderContext context) 
        : base(context.GetWinHandle())
    {
        this.context = context;
    }

    internal unsafe static Win32ShaderImpl FromGlsl(Win32RenderContext context, byte* vshSource, byte* fshSource)
    {
        Win32ShaderImpl impl = new(context);
        impl.shaderHandle = Interop.MsdgCreateShaderFromGlsl(impl.winHandle, vshSource, fshSource);
        return impl;
    }

    int IShaderImpl.GetParameterLocation(string name)
    {
        EnsureState();
        EnsureCurrentState();
        return Interop.MsdgGetShaderParamLocation(winHandle, shaderHandle, name);
    }

    int IShaderImpl.GetParameterLocation(ReadOnlySpan<byte> nameUtf8)
    {
        EnsureState();
        EnsureCurrentState();
        unsafe
        {
            fixed (byte* ptr = nameUtf8)
            {
                return Interop.MsdgGetShaderParamLocation(winHandle, shaderHandle, ptr);
            }
        }
    }

    unsafe void IShaderImpl.SetParameter<T>(int location, T value)
    {
        EnsureState();
        EnsureCurrentState();
        // our jit god will do the optimization

        if (typeof(T) == typeof(int))
        {
            Interop.MsdgSetShaderParamInt(winHandle, location, (int)(object)value);
            return;
        }

        if (typeof(T) == typeof(float))
        {
            Interop.MsdgSetShaderParamFloat(winHandle, location, (float)(object)value);
            return;
        }

        if (typeof(T) == typeof(Vector4))
        {
            Vector4 vec = (Vector4)(object)value;
            Interop.MsdgSetShaderParamVec4(winHandle, location, &vec);
        }

        throw new NotSupportedException($"Type of {typeof(T)} is not supported in shader parameter.");
    }

    internal IntPtr GetShaderHandle()
    {
        EnsureState();
        return shaderHandle;
    }

    private void EnsureCurrentState()
    {
        ThrowHelper.ThrowIfInvalid(context.GetCurrentShader()?.GetImpl() != this, "This operation required this shader to be current.");
    }

    protected override void Dispose(bool disposing)
    {
        base.Dispose(disposing);
        throw new NotImplementedException();
    }
}