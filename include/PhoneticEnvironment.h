/**
 * @brief 音韻変化の条件
 *
 */
enum PhoneticEnvironment
{
    UnDefined = -1,
    // 語頭
    Start = 0,
    // 語中
    Middle = 1,
    // 語尾
    End = 2
};

inline const PhoneticEnvironment ConvertToPhoneticEnvironment(const int i)
{
    switch (i)
    {
    case 0:
        return PhoneticEnvironment::Start;
    case 1:
        return PhoneticEnvironment::Middle;
    case 2:
        return PhoneticEnvironment::End;
    default:
        return PhoneticEnvironment::UnDefined;
    }
}
inline const int ConvertFromPhoneticEnvironment(const PhoneticEnvironment env)
{
    switch (env)
    {
    case PhoneticEnvironment::Start:
        return 0;
    case PhoneticEnvironment::Middle:
        return 1;
    case PhoneticEnvironment::End:
        return 2;
    default:
        return -1;
    }
}