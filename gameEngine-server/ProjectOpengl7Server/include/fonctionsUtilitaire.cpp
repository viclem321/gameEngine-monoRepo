// SequenceDiff : return the diff position inside a sequence uint16
inline bool IsSequenceNewer(uint16_t sNew, uint16_t sLast)
{
    if (sNew == sLast)
        return false;
    return (sNew > sLast && sNew - sLast <= (std::numeric_limits<uint16_t>::max)() / 2)
        || (sNew < sLast && sLast - sNew > (std::numeric_limits<uint16_t>::max)() / 2);
}
uint16_t SequenceDiff(uint16_t sNew, uint16_t sLast)
{
    if (sNew == sLast)
        return 0;
    
    if(IsSequenceNewer(sNew, sLast))
    {
        if (sNew > sLast && sNew - sLast <= (std::numeric_limits<uint16_t>::max)() / 2)
            return sNew - sLast;
        return ((std::numeric_limits<uint16_t>::max)() - sLast) + sNew + 1;
    }
    else
    {
        if (sLast > sNew && sLast - sNew <= (std::numeric_limits<uint16_t>::max)() / 2)
            return (std::numeric_limits<uint16_t>::max)() / 2 + (sLast - sNew);
        return (std::numeric_limits<uint16_t>::max)() / 2 + (((std::numeric_limits<uint16_t>::max)() - sLast) + sLast + 1);
    }
}