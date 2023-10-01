﻿namespace Monosand;

public readonly struct ShaderParameter : IEquatable<ShaderParameter>
{
    private readonly Shader shader;
    private readonly int location;

    public readonly Shader Shader => shader;
    internal readonly int Location => location;

    internal ShaderParameter(Shader shader, int location)
    {
        this.shader = shader;
        this.location = location;
    }

    public void Set<T>(T value) where T : unmanaged
        => shader.SetParameter(in this, value);

    #region Equality

    public readonly bool Equals(ShaderParameter other)
        => other.shader == shader && other.location == location;

    public readonly override bool Equals(object? obj)
        => obj is ShaderParameter s && Equals(s);

    public static bool operator ==(ShaderParameter left, ShaderParameter right)
        => left.Equals(right);

    public static bool operator !=(ShaderParameter left, ShaderParameter right)
        => !(left == right);

    public readonly override int GetHashCode()
        => HashCode.Combine(shader, location);

    #endregion
}