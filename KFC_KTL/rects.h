#ifndef rects_h
#define rects_h

// ------
// Rects
// ------
template <class t>
class TRects : public TList<TRect<t> >
{
public:
    TRects() {}

    TRects(const TRect<t>& Start)
    {
        *TList<TRect<t> >::AddLast() = Start;
    }

    TRects<t>& operator -= (const TRect<t>& Rect);
};

template <class t>
TRects<t>& TRects<t>::operator -= (const TRect<t>& Rect)
{
    if(TList<TRect<t> >::IsEmpty())
        return *this;

    typename TList<TRect<t> >::TIterator LastIter = TList<TRect<t> >::GetLast();

    typename TList<TRect<t> >::TIterator Iter, NIter;

    for(Iter = TList<TRect<t> >::GetFirst() ; ; Iter = NIter)
    {
        NIter = Iter.GetNext();

        const bool bLast = Iter == LastIter;

        IRECT ResRects[4];
        const size_t szN = SubtractRect(*Iter, Rect, ResRects);

        size_t i;
        for(i = 0 ; i < szN ; i++)
            *TList<TRect<t> >::AddLast() = ResRects[i];

        Del(Iter);

        if(bLast)
            break;
    }

    return *this;
}

#endif // rects_h
