 int MainWindow::miniMax(Node *_root, int _dep, int _past_dep, Game_pole _pole, unsigned char _past_lvl, int _alfa, int _beta)
{
    if (_dep == _past_dep || isWin(_root->node_pole) || isLose(_root->node_pole))
    {
        _root->cnt_son = 0;
        _root->nodes = NULL;
        _root->value = evalGame(_root->node_pole);

        if (isWin(_root->node_pole))
        {
            _root->win_lose = WIN;
        }
        else if (isLose(_root->node_pole))
        {
            _root->win_lose = LOSE;
        }
        else
        {
            _root->win_lose = NON;
        }

        return _root->value;
    }
    else
    {
        int beta = _beta;
        int alfa = _alfa;

        int idx;
        unsigned char level;

        if (NONE == _past_lvl || MINI == _past_lvl)
        {
            level = MAXI;
        }
        else if (MAXI == _past_lvl)
        {
            level = MINI;
        }

        clearPossMovs(SIZE_POSS_MOVES);


        if (MAXI == level)
        {
            _root->cnt_son = countPossibleMovesMAXI(_root->node_pole);
            qDebug("MAXI: sons - %u", _root->cnt_son);
        }
        else if (MINI == level)
        {
            _root->cnt_son = countPossibleMovesMINI(_root->node_pole);
            qDebug("MINI: sons - %u", _root->cnt_son);
        }

        if (0 == _root->cnt_son)
        {
            _root->nodes=NULL;
            return 0;
        }

        _root->nodes = new Node*[_root->cnt_son];
        for (idx = 0; idx < _root->cnt_son; idx++)
        {
            Game_pole temp_pole = _root->node_pole;

            _root->nodes[idx] = new Node;

            _root->nodes[idx]->item[0].x = possible_moves[0][idx].x;
            _root->nodes[idx]->item[0].y = possible_moves[0][idx].y;
            _root->nodes[idx]->item[1].x = possible_moves[1][idx].x;
            _root->nodes[idx]->item[1].y = possible_moves[1][idx].y;

            _root->nodes[idx]->node_pole = setShash(_root->nodes[idx]->item[1].x,
                                                    _root->nodes[idx]->item[1].y,
                                                    _root->nodes[idx]->item[0].x,
                                                    _root->nodes[idx]->item[0].y,
                                                    temp_pole, BOT);

            _root->nodes[idx]->value = 0;
            _root->nodes[idx]->flag = false;
            _root->nodes[idx]->level = level;
            _root->nodes[idx]->dis = _past_dep + 1;
            _root->nodes[idx]->win_lose = NON;

        }

        clearPossMovs(_root->cnt_son);

        for (idx = 0; idx < _root->cnt_son; idx++)
        {
            _root->nodes[idx]->value = miniMax(_root->nodes[idx], _dep, _root->nodes[idx]->dis,
                                               _root->nodes[idx]->node_pole, level, alfa, beta);

            if (MAXI == _root->nodes[idx]->level)
            {
                if (_root->nodes[idx]->value > alfa)
                {
                    alfa = _root->nodes[idx]->value;
                    qDebug("ALFA is := %u", alfa);
                }

                if (WIN == _root->nodes[idx]->win_lose)
                {
                    _root->nodes[idx]->win_lose = WIN;
                    return _root->nodes[idx]->value;
                }
            }
            else if (MINI == _root->nodes[idx]->level)
            {
                if (_root->nodes[idx]->value < beta)
                {
                    beta = _root->nodes[idx]->value;
                    qDebug("BETA is := %u___%u", beta, _root->nodes[idx]->value);
                }

                if (LOSE == _root->nodes[idx]->win_lose)
                {
                    _root->nodes[idx]->win_lose = LOSE;
                    return _root->nodes[idx]->value;
                }
            }
            if (alfa >= beta)
            {
                qDebug("ALFA > BETA");
                break;
            }
        }

        int res = 0;

        if (MAXI == _root->level)
        {
            res = beta;
        }
        else if (MINI == _root->level)
        {
            res = alfa;
        }

        return res;
    }
}