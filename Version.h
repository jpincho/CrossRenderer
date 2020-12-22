#pragma once
#include <string.h>
#include <stdio.h>
#include <CrossRendererConfig.h>
class Version
    {
    public:
        unsigned Minor, Major;

        Version ( void )
            {
            Major = Minor = 0;
            }
        void Set ( unsigned NewMajor, unsigned NewMinor )
            {
            Major = NewMajor;
            Minor = NewMinor;
            }
        bool ParseString ( const char *String )
            {
            // Safe values
            Major = 0;
            Minor = 0;

            const char *DotPosition = strchr ( String, '.' );
            if ( DotPosition == nullptr )
                return false;

            const char *VersionStart;
            for ( VersionStart = DotPosition; ( VersionStart > String ) && ( *VersionStart != ' ' ); --VersionStart );
            if ( *VersionStart != ' ' ) // If there was nothing before the version numbers, assume the whole string
                VersionStart = String;

#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
            sscanf_s ( VersionStart, "%u.%u", &Major, &Minor );
#else
            sscanf ( VersionStart, "%u.%u", &Major, &Minor );
#endif
            return true;
            }
        bool operator < ( const Version &Other )
            {
            return Lesser ( Other.Major, Other.Minor );
            }
        bool operator <= ( const Version &Other )
            {
            return LesserEqual ( Other.Major, Other.Minor );
            }
        bool operator > ( const Version &Other )
            {
            return Greater ( Other.Major, Other.Minor );
            }
        bool operator >= ( const Version &Other )
            {
            return GreaterEqual ( Other.Major, Other.Minor );
            }
        bool Lesser ( const unsigned OtherMajor, const unsigned OtherMinor )
            {
            return ( ( Major < OtherMajor ) ||
                     ( ( Major == OtherMajor ) && ( Minor < OtherMinor ) ) );
            }
        bool Greater ( const unsigned OtherMajor, const unsigned OtherMinor )
            {
            return ( ( Major > OtherMajor ) ||
                     ( ( Major == OtherMajor ) && ( Minor > OtherMinor ) ) );
            }
        bool LesserEqual ( const unsigned OtherMajor, const unsigned OtherMinor )
            {
            return ( Equal ( OtherMajor, OtherMinor ) ) || ( Lesser ( OtherMajor, OtherMinor ) );
            }
        bool GreaterEqual ( const unsigned OtherMajor, const unsigned OtherMinor )
            {
            return ( Equal ( OtherMajor, OtherMinor ) ) || ( Greater ( OtherMajor, OtherMinor ) );
            }
        bool Equal ( const unsigned OtherMajor, const unsigned OtherMinor )
            {
            return ( ( Major == OtherMajor ) && ( Minor == OtherMinor ) );
            }
    };
