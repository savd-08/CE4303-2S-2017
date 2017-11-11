package com.example.abcbr.osnumericalkeyboard.adapters;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

/**
 * Created by abcbr on 15/10/2017.
 */

public class SectionsPagerAdapter extends FragmentPagerAdapter {

    String mTitles[];

    Fragment mFragments[];

    public SectionsPagerAdapter(FragmentManager fm, Fragment[] pFragments, String[] pTitles ){
        super(fm);
        mTitles = pTitles;
        mFragments = pFragments;
    }

    @Override
    public Fragment getItem(int position) {
        return mFragments[position];
    }

    @Override
    public int getCount() {
        return mFragments.length;
    }

    @Override
    public CharSequence getPageTitle(int position) {
        return mTitles[position];
    }
}
