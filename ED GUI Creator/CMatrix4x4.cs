using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ED_GUI_Creator
{
    class CMatrix4x4
    {
        float[,] m_Matrix;
        
        public float[,] Matrix
        {
            get { return m_Matrix; }
            set { m_Matrix = value; }
        }

        public CMatrix4x4()
        {
            m_Matrix = new float[4, 4];
            Identity();
        }

        public void Identity()
        {
            // X Axis
            m_Matrix[0, 0] = 1.0f;
            m_Matrix[0, 1] = 0.0f;
            m_Matrix[0, 2] = 0.0f;
            m_Matrix[0, 3] = 0.0f;

            // Y Axis
            m_Matrix[1, 0] = 0.0f;
            m_Matrix[1, 1] = 1.0f;
            m_Matrix[1, 2] = 0.0f;
            m_Matrix[1, 3] = 0.0f;

            // Z Axis
            m_Matrix[2, 0] = 0.0f;
            m_Matrix[2, 1] = 0.0f;
            m_Matrix[2, 2] = 1.0f;
            m_Matrix[2, 3] = 0.0f;

            // W Axis
            m_Matrix[3, 0] = 0.0f;
            m_Matrix[3, 1] = 0.0f;
            m_Matrix[3, 2] = 0.0f;
            m_Matrix[3, 3] = 1.0f;
        }

        public void Translate(float _X = 0.0f, float _Y = 0.0f, float _Z = 0.0f)
        {
            m_Matrix[3, 0] += _X;
            m_Matrix[3, 1] += _Y;
            m_Matrix[3, 2] += _Z;
        }

        public CMatrix4x4 MakeRotationX(float _Radians)
        {
            float sinAngle = (float)Math.Sin(_Radians);
            float cosAngle = (float)Math.Cos(_Radians);

            CMatrix4x4 newMatrix = new CMatrix4x4();

            SetIndex(1, 1, cosAngle);
            SetIndex(1, 2, sinAngle);
            SetIndex(2, 1, -sinAngle);
            SetIndex(2, 2, cosAngle);

            return newMatrix;
        }

        public CMatrix4x4 MakeRotationY(float _Radians)
        {
            float sinAngle = (float)Math.Sin(_Radians);
            float cosAngle = (float)Math.Cos(_Radians);

            CMatrix4x4 newMatrix = new CMatrix4x4();

            SetIndex(0, 0, cosAngle);
            SetIndex(0, 2, -sinAngle);
            SetIndex(2, 0, sinAngle);
            SetIndex(2, 2, cosAngle);

            return newMatrix;
        }

        public CMatrix4x4 MakeRotationZ(float _Radians)
        {
            float sinAngle = (float)Math.Sin(_Radians);
            float cosAngle = (float)Math.Cos(_Radians);

            CMatrix4x4 newMatrix = new CMatrix4x4();

            SetIndex(0, 0, cosAngle);
            SetIndex(0, 1, sinAngle);
            SetIndex(1, 0, -sinAngle);
            SetIndex(1, 1, cosAngle);

            return newMatrix;
        }

        public void RotateLocalX(float _Radians)
        {
            CMatrix4x4 rotMatrix = new CMatrix4x4();
            rotMatrix = rotMatrix.MakeRotationX(_Radians);

            m_Matrix = Multiply(rotMatrix.m_Matrix, m_Matrix);
        }

        public void RotateLocalY(float _Radians)
        {
            CMatrix4x4 rotMatrix = new CMatrix4x4();
            rotMatrix = rotMatrix.MakeRotationY(_Radians);

            m_Matrix = Multiply(rotMatrix.m_Matrix, m_Matrix);
        }

        public void RotateLocalZ(float _Radians)
        {
            CMatrix4x4 rotMatrix = new CMatrix4x4();
            rotMatrix = rotMatrix.MakeRotationZ(_Radians);

            m_Matrix = Multiply(rotMatrix.m_Matrix, m_Matrix);
        }

        public void Scale(float _X = 1.0f, float _Y = 1.0f, float _Z = 1.0f)
        {
            // X Axis
            m_Matrix[0, 0] *= _X;
            m_Matrix[0, 1] *= _X;
            m_Matrix[0, 2] *= _X;
            m_Matrix[0, 3] *= _X;

            // Y Axis
            m_Matrix[1, 0] *= _Y;
            m_Matrix[1, 1] *= _Y;
            m_Matrix[1, 2] *= _Y;
            m_Matrix[1, 3] *= _Y;

            // Z Axis
            m_Matrix[2, 0] *= _Z;
            m_Matrix[2, 1] *= _Z;
            m_Matrix[2, 2] *= _Z;
            m_Matrix[2, 3] *= _Z;

        }

        public CMatrix4x4 Multiply(CMatrix4x4 _First, CMatrix4x4 _Second)
        {
            CMatrix4x4 newMatrix = new CMatrix4x4();
            float[,] first = _First.m_Matrix;
            float[,] second = _Second.m_Matrix;

            // X Axis
            newMatrix.m_Matrix[0, 0] = first[0, 0] * second[0, 0] +
                                        first[0, 1] * second[1, 0] +
                                        first[0, 2] * second[2, 0] +
                                        first[0, 3] * second[3, 0];
            newMatrix.m_Matrix[0, 1] = first[0, 0] * second[0, 1] +
                                        first[0, 1] * second[1, 1] +
                                        first[0, 2] * second[2, 1] +
                                        first[0, 3] * second[3, 1];
            newMatrix.m_Matrix[0, 2] = first[0, 0] * second[0, 2] +
                                        first[0, 1] * second[1, 2] +
                                        first[0, 2] * second[2, 2] +
                                        first[0, 3] * second[3, 2];
            newMatrix.m_Matrix[0, 3] = first[0, 0] * second[0, 3] +
                                        first[0, 1] * second[1, 3] +
                                        first[0, 2] * second[2, 3] +
                                        first[0, 3] * second[3, 3];

            // Y Axis
            newMatrix.m_Matrix[1, 0] = first[1, 0] * second[0, 0] +
                                        first[1, 1] * second[1, 0] +
                                        first[1, 2] * second[2, 0] +
                                        first[1, 3] * second[3, 0];
            newMatrix.m_Matrix[1, 1] = first[1, 0] * second[0, 1] +
                                        first[1, 1] * second[1, 1] +
                                        first[1, 2] * second[2, 1] +
                                        first[1, 3] * second[3, 1];
            newMatrix.m_Matrix[1, 2] = first[1, 0] * second[0, 2] +
                                        first[1, 1] * second[1, 2] +
                                        first[1, 2] * second[2, 2] +
                                        first[1, 3] * second[3, 2];
            newMatrix.m_Matrix[1, 3] = first[1, 0] * second[0, 3] +
                                        first[1, 1] * second[1, 3] +
                                        first[1, 2] * second[2, 3] +
                                        first[1, 3] * second[3, 3];

            // Z Axis
            newMatrix.m_Matrix[2, 0] = first[2, 0] * second[0, 0] +
                                        first[2, 1] * second[1, 0] +
                                        first[2, 2] * second[2, 0] +
                                        first[2, 3] * second[3, 0];
            newMatrix.m_Matrix[2, 1] = first[2, 0] * second[0, 1] +
                                        first[2, 1] * second[1, 1] +
                                        first[2, 2] * second[2, 1] +
                                        first[2, 3] * second[3, 1];
            newMatrix.m_Matrix[2, 2] = first[2, 0] * second[0, 2] +
                                        first[2, 1] * second[1, 2] +
                                        first[2, 2] * second[2, 2] +
                                        first[2, 3] * second[3, 2];
            newMatrix.m_Matrix[2, 3] = first[2, 0] * second[0, 3] +
                                        first[2, 1] * second[1, 3] +
                                        first[2, 2] * second[2, 3] +
                                        first[2, 3] * second[3, 3];

            // W Axis
            newMatrix.m_Matrix[3, 0] = first[3, 0] * second[0, 0] +
                                        first[3, 1] * second[1, 0] +
                                        first[3, 2] * second[2, 0] +
                                        first[3, 3] * second[3, 0];
            newMatrix.m_Matrix[3, 1] = first[3, 0] * second[0, 1] +
                                        first[3, 1] * second[1, 1] +
                                        first[3, 2] * second[2, 1] +
                                        first[3, 3] * second[3, 1];
            newMatrix.m_Matrix[3, 2] = first[3, 0] * second[0, 2] +
                                        first[3, 1] * second[1, 2] +
                                        first[3, 2] * second[2, 2] +
                                        first[3, 3] * second[3, 2];
            newMatrix.m_Matrix[3, 3] = first[3, 0] * second[0, 3] +
                                        first[3, 1] * second[1, 3] +
                                        first[3, 2] * second[2, 3] +
                                        first[3, 3] * second[3, 3];

            return newMatrix;
        }

        // ASSUMES THESE ARE BOTH FLOAT[4, 4]!!
        public float[,] Multiply(float[,] _First, float[,] _Second)
        {
            float[,] newMatrix = new float[4, 4];

            // X Axis
            newMatrix[0, 0] = _First[0, 0] * _Second[0, 0] +
                                        _First[0, 1] * _Second[1, 0] +
                                        _First[0, 2] * _Second[2, 0] +
                                        _First[0, 3] * _Second[3, 0];
            newMatrix[0, 1] = _First[0, 0] * _Second[0, 1] +
                                        _First[0, 1] * _Second[1, 1] +
                                        _First[0, 2] * _Second[2, 1] +
                                        _First[0, 3] * _Second[3, 1];
            newMatrix[0, 2] = _First[0, 0] * _Second[0, 2] +
                                        _First[0, 1] * _Second[1, 2] +
                                        _First[0, 2] * _Second[2, 2] +
                                        _First[0, 3] * _Second[3, 2];
            newMatrix[0, 3] = _First[0, 0] * _Second[0, 3] +
                                        _First[0, 1] * _Second[1, 3] +
                                        _First[0, 2] * _Second[2, 3] +
                                        _First[0, 3] * _Second[3, 3];

            // Y Axis
            newMatrix[1, 0] = _First[1, 0] * _Second[0, 0] +
                                        _First[1, 1] * _Second[1, 0] +
                                        _First[1, 2] * _Second[2, 0] +
                                        _First[1, 3] * _Second[3, 0];
            newMatrix[1, 1] = _First[1, 0] * _Second[0, 1] +
                                        _First[1, 1] * _Second[1, 1] +
                                        _First[1, 2] * _Second[2, 1] +
                                        _First[1, 3] * _Second[3, 1];
            newMatrix[1, 2] = _First[1, 0] * _Second[0, 2] +
                                        _First[1, 1] * _Second[1, 2] +
                                        _First[1, 2] * _Second[2, 2] +
                                        _First[1, 3] * _Second[3, 2];
            newMatrix[1, 3] = _First[1, 0] * _Second[0, 3] +
                                        _First[1, 1] * _Second[1, 3] +
                                        _First[1, 2] * _Second[2, 3] +
                                        _First[1, 3] * _Second[3, 3];

            // Z Axis
            newMatrix[2, 0] = _First[2, 0] * _Second[0, 0] +
                                        _First[2, 1] * _Second[1, 0] +
                                        _First[2, 2] * _Second[2, 0] +
                                        _First[2, 3] * _Second[3, 0];
            newMatrix[2, 1] = _First[2, 0] * _Second[0, 1] +
                                        _First[2, 1] * _Second[1, 1] +
                                        _First[2, 2] * _Second[2, 1] +
                                        _First[2, 3] * _Second[3, 1];
            newMatrix[2, 2] = _First[2, 0] * _Second[0, 2] +
                                        _First[2, 1] * _Second[1, 2] +
                                        _First[2, 2] * _Second[2, 2] +
                                        _First[2, 3] * _Second[3, 2];
            newMatrix[2, 3] = _First[2, 0] * _Second[0, 3] +
                                        _First[2, 1] * _Second[1, 3] +
                                        _First[2, 2] * _Second[2, 3] +
                                        _First[2, 3] * _Second[3, 3];

            // W Axis
            newMatrix[3, 0] = _First[3, 0] * _Second[0, 0] +
                                        _First[3, 1] * _Second[1, 0] +
                                        _First[3, 2] * _Second[2, 0] +
                                        _First[3, 3] * _Second[3, 0];
            newMatrix[3, 1] = _First[3, 0] * _Second[0, 1] +
                                        _First[3, 1] * _Second[1, 1] +
                                        _First[3, 2] * _Second[2, 1] +
                                        _First[3, 3] * _Second[3, 1];
            newMatrix[3, 2] = _First[3, 0] * _Second[0, 2] +
                                        _First[3, 1] * _Second[1, 2] +
                                        _First[3, 2] * _Second[2, 2] +
                                        _First[3, 3] * _Second[3, 2];
            newMatrix[3, 3] = _First[3, 0] * _Second[0, 3] +
                                        _First[3, 1] * _Second[1, 3] +
                                        _First[3, 2] * _Second[2, 3] +
                                        _First[3, 3] * _Second[3, 3];

            return newMatrix;
        }

        ////////////////////////////////////////////////////////////
        // Accessors
        ////////////////////////////////////////////////////////////
        public float GetIndex(int _Index1, int _Index2)
        {
            return m_Matrix[_Index1, _Index2];
        }

        ////////////////////////////////////////////////////////////
        // Mutators
        ////////////////////////////////////////////////////////////
        public void SetIndex(int _Index1, int _Index2, float _Value)
        {
            m_Matrix[_Index1, _Index2] = _Value;
        }
    }
}
