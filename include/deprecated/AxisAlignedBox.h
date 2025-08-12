#pragma once

class AxisAlignedBox
{
public:
    explicit AxisAlignedBox( const D3DXVECTOR2 & leftTop=D3DXVECTOR2(0, 0), const D3DXVECTOR2 & rightBottom=D3DXVECTOR2(0, 0))
        : mLeftTop(leftTop),
          mRightBottom(rightBottom)
    {
        assert(isValid());
    }

    AxisAlignedBox( const AxisAlignedBox & axisAlignedBox )
        : mLeftTop(axisAlignedBox.mLeftTop),
          mRightBottom(axisAlignedBox.mRightBottom)
    {
        assert(isValid());
    }

    AxisAlignedBox & operator=( const AxisAlignedBox & axisAlignedBox )
    {
        if (this != &axisAlignedBox)
        {
            mLeftTop = axisAlignedBox.mLeftTop;
            mRightBottom = axisAlignedBox.mRightBottom;
        }
        return *this;
    }

    AxisAlignedBox & operator+=( const AxisAlignedBox & axisAlignedBox )
    {
        return inflate(axisAlignedBox);
    }

    AxisAlignedBox & operator+( const AxisAlignedBox & axisAlignedBox )
    {
        return AxisAlignedBox(*this).inflate(axisAlignedBox);
    }

    AxisAlignedBox & operator+=( const D3DXVECTOR2 & point )
    {
        return inflate(point);
    }

    AxisAlignedBox & operator+( const D3DXVECTOR2 & point )
    {
        return AxisAlignedBox(*this).inflate(point);
    }

    const D3DXVECTOR2 & getLeftTop( void ) const
    {
        return mLeftTop;
    }

    const D3DXVECTOR2 & getRightBottom( void ) const
    {
        return mRightBottom;
    }

    D3DXVECTOR2 getSize( void ) const
    {
        assert(isValid());

        return D3DXVECTOR2(mRightBottom - mLeftTop);
    }

    bool isInside( const D3DXVECTOR2 & point ) const
    {
        assert(isValid());

        return ((point.x > mLeftTop.x && point.x < mRightBottom.x) &&
                (point.y > mLeftTop.y && point.y < mRightBottom.y));
    }

    bool isValid( void ) const
    {
        return (mLeftTop.x <= mRightBottom.x && mLeftTop.y <= mRightBottom.y);
    }

    bool isIntersect( const AxisAlignedBox & axisAlignedBox ) const
    {
        assert(isValid() && axisAlignedBox.isValid());

        D3DXVECTOR2 size = getIntersection(axisAlignedBox).getSize();
        return (size.x > 0 && size.y > 0);
    }

    AxisAlignedBox getIntersection( const AxisAlignedBox & axisAlignedBox ) const
    {
        assert(isValid() && axisAlignedBox.isValid());

        D3DXVECTOR2 d1 = axisAlignedBox.mRightBottom - mLeftTop;
        D3DXVECTOR2 d2 = mRightBottom - axisAlignedBox.mLeftTop;

        D3DXVECTOR2 leftTop(0, 0);
        D3DXVECTOR2 rightBottom(0, 0);

        if (d1.x > 0.0f && d1.y > 0.0f && d2.x > 0.0f && d2.y > 0.0f)
        {
            leftTop.x = std::max(mLeftTop.x, axisAlignedBox.mLeftTop.x);
            leftTop.y = std::max(mLeftTop.y, axisAlignedBox.mLeftTop.y);

            rightBottom.x = std::min(mRightBottom.x, axisAlignedBox.mRightBottom.x);
            rightBottom.y = std::min(mRightBottom.y, axisAlignedBox.mRightBottom.y);
        }

        return AxisAlignedBox(leftTop, rightBottom);
    }

    AxisAlignedBox & transform( const D3DXMATRIX & transformation )
    {
        D3DXVECTOR4 p;

        D3DXVec2Transform(&p, &mLeftTop, &transformation);
        mLeftTop.x = p.x;
        mLeftTop.y = p.y;

        D3DXVec2Transform(&p, &mRightBottom, &transformation);
        mRightBottom.x = p.x;
        mRightBottom.y = p.y;

        return *this;
    }

private:
    AxisAlignedBox & inflate( const AxisAlignedBox & axisAlignedBox )
    {
        assert(isValid() && axisAlignedBox.isValid());

        return inflate(axisAlignedBox.mLeftTop).inflate(axisAlignedBox.mRightBottom);
    }

    AxisAlignedBox & inflate( const D3DXVECTOR2 & point )
    {
        assert(isValid());

        mLeftTop.x = std::min(mLeftTop.x, point.x);
        mLeftTop.y = std::min(mLeftTop.y, point.y);
        mRightBottom.x = std::max(mRightBottom.x, point.x);
        mRightBottom.y = std::max(mRightBottom.y, point.y);

        return *this;
    }

    D3DXVECTOR2 mLeftTop;
    D3DXVECTOR2 mRightBottom;
};
