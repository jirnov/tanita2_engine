/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Trickster Games, 2008. Licensed under GPL license.

 ��� (������� ������� �������� ��������).
 */
#pragma once
#ifndef _FLAMENCO_CORE_PIN_H_
#define _FLAMENCO_CORE_PIN_H_

namespace flamenco
{

// ��� (������� ������� �������� ��������).
class pin : public refcountable
{
public:
    // ����������� �� ��� � ������� ���� ��� �������.
    inline bool connected() const
    {
        return mIsConnected;
    }

protected:
    pin();
    virtual ~pin() = 0;
    friend class reference<pin>;

    // ��������� ������ ������� ��������� �������.
    // ������ ������� ������ - CHANNEL_BUFFER_SIZE_IN_SAMPLES �������.
    virtual void process( f32 * left, f32 * right ) = 0;

private:
    bool mIsConnected;

    // ����������� � ������������� ��� ������������ ������� ���� � �������.
    virtual void on_attach( bool /* connected */ ) {};

    // �������� ��� ��� �������������� ��� �������������.
    inline void set_connected( bool connected )
    {
        mIsConnected = connected;
    }

    friend class effect;
    friend class mixer;
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_PIN_H_
